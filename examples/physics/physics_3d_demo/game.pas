{
  Copyright 2017-2017 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Implements the game logic. }
unit Game;

interface

implementation

uses SysUtils, Classes, Generics.Collections,
  CastleWindow, CastleScene, CastleControls, CastleLog, X3DNodes, Castle3D,
  CastleFilesUtils, CastleSceneCore, CastleKeysMouse, CastleColors,
  CastleCameras, CastleVectors, CastleRenderer, CastleBoxes, CastleSceneManager,
  CastleUIControls;

var
  Window: TCastleWindow;
  SceneManager: TCastleSceneManager; //< Shortcut for Window.SceneManager
  LevelScene: TCastleScene;
  Level: T3DTransform;
  BoxTemplate, SphereTemplate: TCastleScene;

procedure LoadLevel(const URL: string; const MeshCollider: boolean);

  function CreatePlaneCollider(const ParentBody: TRigidBody): TPlaneCollider;
  begin
    Result := TPlaneCollider.Create(ParentBody);
    Result.Normal := Vector3(0, 1, 0);
    Result.Distance := 0;
    Result.Restitution := 0.3;
  end;

  function CreateMeshCollider(const ParentBody: TRigidBody): TMeshCollider;
  begin
    Result := TMeshCollider.Create(ParentBody);
    Result.Scene := LevelScene;
    Result.Restitution := 0.3;
  end;

var
  LevelBody: TRigidBody;
  MoveLimit: TBox3D;
begin
  { free previous level, which also frees all related rigid bodies }
  FreeAndNil(Level);
  FreeAndNil(LevelScene);

  // SceneManager.Items.Clear; // not needed, we already freed everything
  SceneManager.ClearCameras; // recreate new camera for new level

  { TODO:
    This code will be simpler once we merge various T3D descendants
    into TCastleTransform,
    and make TCastleScene descend from TCastleTransform.
    Coming in next CGE release (6.4),
    see https://castle-engine.sourceforge.io/planned_features.php }

  LevelScene := TCastleScene.Create(Application);
  LevelScene.Load(URL);
  LevelScene.Spatial := [ssRendering, ssDynamicCollisions];
  LevelScene.ProcessEvents := true;
  LevelScene.Attributes.Shaders := srAlways; // nicer lighting

  Level := T3DTransform.Create(Application);
  Level.Add(LevelScene);

  LevelBody := TRigidBody.Create(LevelScene);
  LevelBody.Dynamic := false;

  if MeshCollider then
    CreateMeshCollider(LevelBody)
  else
    CreatePlaneCollider(LevelBody);

  { assign this only once LevelBody and LevelCollider
    are fully configured, this initializes physics engine }
  Level.RigidBody := LevelBody;

  SceneManager.Items.Add(Level);
  SceneManager.MainScene := LevelScene;

  // make gravity work even if your position is over the world bbox
  MoveLimit := SceneManager.Items.BoundingBox;
  MoveLimit.Max := MoveLimit.Max + Vector3(0, 1000, 0);
  SceneManager.MoveLimit := MoveLimit;
end;

type
  TEventHandler = class
    class procedure LoadLevelSimple(Sender: TObject);
    class procedure LoadLevelComplex(Sender: TObject);
  end;

class procedure TEventHandler.LoadLevelSimple(Sender: TObject);
begin
  LoadLevel(ApplicationData('level_simple.x3dv'), false);
end;

class procedure TEventHandler.LoadLevelComplex(Sender: TObject);
begin
  LoadLevel(ApplicationData('level_complex.x3dv'), true);
end;

{ One-time initialization of resources. }
procedure ApplicationInitialize;
var
  ButtonLevelSimple, ButtonLevelComplex: TCastleButton;
begin
  SceneManager := Window.SceneManager;

  LoadLevel(ApplicationData('level_simple.x3dv'), false);

  SceneManager.NavigationType := ntWalk;
  // rotating by dragging would cause trouble when clicking to spawn boxes/spheres
  SceneManager.WalkCamera.Input :=
    SceneManager.WalkCamera.Input - [ciMouseDragging];
  SceneManager.WalkCamera.HeadBobbing := 0; // looks bad
  // easy way to make the simulation feel more dynamic
  SceneManager.TimeScale := 2;

  BoxTemplate := TCastleScene.Create(Application);
  BoxTemplate.Load(ApplicationData('box.x3d'));

  SphereTemplate := TCastleScene.Create(Application);
  SphereTemplate.Load(ApplicationData('sphere.x3d'));

  Window.Container.UIReferenceWidth := 1024;
  Window.Container.UIReferenceHeight := 768;
  Window.Container.UIScaling := usEncloseReferenceSize;

  ButtonLevelSimple := TCastleButton.Create(Application);
  ButtonLevelSimple.Caption := 'Simple Level (Plane Collider)';
  ButtonLevelSimple.OnClick := @TEventHandler(nil).LoadLevelSimple;
  ButtonLevelSimple.Anchor(hpLeft, 10);
  ButtonLevelSimple.Anchor(vpTop, -10);
  Window.Controls.InsertFront(ButtonLevelSimple);

  ButtonLevelComplex := TCastleButton.Create(Application);
  ButtonLevelComplex.Caption := 'Complex Level (Mesh Collider)';
  ButtonLevelComplex.OnClick := @TEventHandler(nil).LoadLevelComplex;
  ButtonLevelComplex.Anchor(hpLeft, 10);
  ButtonLevelComplex.Anchor(vpTop, -10 - ButtonLevelSimple.CalculatedHeight - 10);
  Window.Controls.InsertFront(ButtonLevelComplex);
end;

procedure WindowRender(Container: TUIContainer);
begin
  UIFont.PrintStrings(10, 10, Yellow, [
    Format('FPS: %f', [Container.Fps.RealTime]),
    'Left mouse button - spawn box',
    'Right mouse button - spawn sphere',
    'AWSD, arrows - move, rotate',
    'F4 - toggle mouse look'
  ], false, 0);
end;

procedure WindowPress(Container: TUIContainer; const Event: TInputPressRelease);

  procedure Spawn(const Template: TCastleScene; const Collider: TCollider;
    const RigidBody: TRigidBody);
  var
    Scene: TCastleScene;
    Transform: T3DTransform;
    CameraPos, CameraDir, CameraUp: TVector3;
  begin
    Scene := Template.Clone(Level);

    Transform := T3DTransform.Create(Level);
    SceneManager.Camera.GetView(CameraPos, CameraDir, CameraUp);
    Transform.Translation := CameraPos + CameraDir * 2.0;
    // TODO: apply Transform.Direction from SceneManager.Camera.Direction
    Transform.Add(Scene);

    SceneManager.Items.Add(Transform);

    RigidBody.InitialLinearVelocity := CameraDir * 4.0;
    Transform.RigidBody := RigidBody;
  end;

var
  C: TWalkCamera;
  RigidBody: TRigidBody;
  BoxCollider: TBoxCollider;
  SphereCollider: TSphereCollider;
begin
  if Event.IsKey(K_F4) then
  begin
    C := SceneManager.WalkCamera;
    C.MouseLook := not C.MouseLook;
  end;

  if Event.IsMouseButton(mbLeft) then
  begin
    RigidBody := TRigidBody.Create(BoxTemplate);

    BoxCollider := TBoxCollider.Create(RigidBody);
    BoxCollider.Size := BoxTemplate.BoundingBox.Size;
    BoxCollider.Restitution := 0.3;
    BoxCollider.Density := 100.0;
    Spawn(BoxTemplate, BoxCollider, RigidBody);
  end;

  if Event.IsMouseButton(mbRight) then
  begin
    RigidBody := TRigidBody.Create(SphereTemplate);

    SphereCollider := TSphereCollider.Create(RigidBody);
    SphereCollider.Radius := SphereTemplate.BoundingBox.Size.X / 2;
    SphereCollider.Friction := 0.4;
    SphereCollider.Restitution := 0.2;
    SphereCollider.Density := 20.0;
    Spawn(SphereTemplate, SphereCollider, RigidBody);
  end;
end;

function MyGetApplicationName: string;
begin
  Result := 'physics_3d_demo';
end;

initialization
  { This sets SysUtils.ApplicationName.
    It is useful to make sure it is correct (as early as possible)
    as our log routines use it. }
  OnGetApplicationName := @MyGetApplicationName;

  InitializeLog;

  { initialize Application callbacks }
  Application.OnInitialize := @ApplicationInitialize;

  { create Window and initialize Window callbacks }
  Window := TCastleWindow.Create(Application);
  Application.MainWindow := Window;
  Window.OnRender := @WindowRender;
  Window.OnPress := @WindowPress;
end.
