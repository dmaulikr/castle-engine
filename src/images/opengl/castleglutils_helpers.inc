{
  Copyright 2016-2017 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Part of CastleGLUtils unit: simple helpers for OpenGL commands. }

{$ifdef read_interface}

{ Comfortable wrappers for OpenGL glGet* that return a single value.

  Guarantee that result is zero in case of OpenGL error.
  (Otherwise, OpenGL could leave them undefined and only set glGetError.)
  @groupBegin }
function glGetFloat(pname: TGLEnum): TGLfloat;
function glGetInteger(pname: TGLEnum): TGLint;
function glGetBoolean(pname: TGLEnum): TGLboolean;
{ @groupEnd }

(*------------------------------------------------------------------------------
  Comfortable wrappers around many OpenGL functions.
  Overloaded for our vector types.

  Note that functions here simply call appropriate OpenGL functions.
  Long time ago we tried using tricks to speed this up (eliminate
  function call overhead), by importing these functions from so/dll
  under different names, like

    procedure glVertexv(const V: TVector3);
      {$ifdef OPENGL_CDECL} cdecl; {$endif}
      {$ifdef OPENGL_STDCALL} stdcall; {$endif}
      overload; external OpenGLDLL name 'glVertex3fv';

  But this is problematic: it assumes that TVector3 will be passed
  by reference. Which actually is not guaranteed by a compiler (FPC sometimes
  doesn't). In newer FPC versions, we could use "constref" for this.
  Or we could just declare these functions as "inline".
  However, speeding these functions is just not needed at all anymore
  (especially with new VBO renderer that passes everything through arrays).

  TODO: Some of these functions should be treated as internal.
  A subset of these internal functions are only available in fixed-function pipeline.
  The rest of these functions are just deprecated -- because they
  are only for fixed-function pipeline, in immediate mode (and all modern code
  should use VBO).
*)

{$ifndef OpenGLES}

// Do not mark glColorv as "deprecated" for now. These have valid (although internal) usage.
{ }
procedure glColorv(const v: TVector3ub); overload; //deprecated 'instead of this, use drawing routines that take Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...) or TGLImage.Color';
{ }
procedure glColorv(const v: TVector4ub); overload; //deprecated 'instead of this, use drawing routines that take Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...) or TGLImage.Color';
{ }
procedure glColorv(const v: TVector3f); overload; //deprecated 'instead of this, use drawing routines that take Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...) or TGLImage.Color';
{ }
procedure glColorv(const v: TVector4f); overload; //deprecated 'instead of this, use drawing routines that take Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...) or TGLImage.Color';

{ }
procedure glTranslatev(const V: TVector3f); overload; deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';
procedure glTranslatev(const V: TVector3_Single); overload; deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';

procedure glScalev(const V: Single); overload; deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';
procedure glScalev(const V: TVector3f); overload; deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';
procedure glScalev(const V: TVector3_Single); overload; deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';

procedure glRotatev(const Angle: TGLfloat;  const V: TVector3f); overload; deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';

procedure glClipPlane(plane: GLenum; const V: TVector4Double); overload;

procedure glNormalv(const v: TVector3); overload;

procedure glMaterialv(face, pname: TGLEnum; const params: TVector4f); overload;

procedure glVertexv(const v: TVector2f); overload;
procedure glVertexv(const v: TVector2i); overload;
procedure glVertexv(const v: TVector3f); overload;
procedure glVertexv(const v: TVector3i); overload;
procedure glVertexv(const v: TVector4f); overload;
procedure glVertexv(const v: TVector4i); overload;

procedure glVertexv(const v: TVector2_Single); overload;
procedure glVertexv(const v: TVector3_Single); overload;
procedure glVertexv(const v: TVector4_Single); overload;

procedure glTexCoordv(const v: TVector2f); overload;
procedure glTexCoordv(const v: TVector3f); overload;
procedure glTexCoordv(const v: TVector4f); overload;

procedure glTexGenv(coord, pname: TGLenum; const params: TVector4f); overload;

procedure glLightv(light, pname: TGLEnum; const params: TVector4f); overload;
procedure glLightv(light, pname: TGLEnum; const params: TVector3f); overload;

procedure glLightModelv(pname: TGLenum; const params: TVector4f); overload;

procedure glFogv(pname: TGLEnum; const params: TVector4f); overload;

// Do not mark as "deprecated" for now. These have valid (although internal) usage.
{ }
procedure glMultMatrix(const m: TMatrix4f); overload; //deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';
{ }
procedure glLoadMatrix(const m: TMatrix4f); overload; //deprecated 'use other methods to transform things, e.g. TUIControl position / anchors, or T3DTransform for TCastleScene, or TTransformNode in X3D';

{ }
procedure glTexEnvv(target, pname: TGLEnum; const params: TVector4f); overload;

{$endif}

procedure GLViewport(const Rect: TRectangle);

var
  { Current color, set by glColorv and used for TCastleFont font printing
    (in case you use deprecated TCastleFont.Print overloads without
    explicit colors).

    @deprecated Instead of this, use drawing routines that take
    Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...)
    or TGLImage.Color. }
  CurrentColor: TCastleColor
    deprecated 'instead of this, use drawing routines that take Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...) or TGLImage.Color';

{ ---------------------------------------------------------------------------- }

{ }
procedure GLSetEnabled(value: TGLenum; isEnabled: boolean);

{$ifndef OpenGLES}

{ Draw vertical line using OpenGL. Uses current OpenGL color.

  Deprecated, do not draw lines directly like this,
  instead use DrawPrimitive2D or UI interface drawing like Theme.Draw and TGLImageCore.Draw. }
procedure GLVerticalLine(x, y1, y2: TGLfloat); deprecated 'use DrawPrimitive2D';

{ Draw horizontal line using OpenGL. Uses current OpenGL color.

  Deprecated, do not draw lines directly like this,
  instead use DrawPrimitive2D or UI interface drawing like Theme.Draw and TGLImageCore.Draw. }
procedure GLHorizontalLine(x1, x2, y: TGLfloat); deprecated 'use DrawPrimitive2D';

{ Draw arrow shape. Arrow is placed on Z = 0 plane, points to the up,
  has height = 2 (from y = 0 to y = 2) and width 1 (from x = -0.5 to 0.5).

  Everything is drawn CCW when seen from standard view (x grows right, y up).
  Uses current OpenGL color. }
procedure GLDrawArrow(HeadThickness: TGLfloat = 0.4;
  HeadLength: TGLfloat = 0.5); deprecated 'use DrawPrimitive2D to draw shapes';

{$ifdef CASTLE_OBJFPC}
{ Comfortable wrapper for gluNewQuadric. Sets all quadric parameters.
  Sets also the GLU_ERROR callback to ReportGLerror.
  @raises Exception If gluNewQuadric fails (returns nil). }
function NewGLUQuadric(
  Texture: boolean = true;
  Normals: TGLenum = GLU_NONE;
  Orientation: TGLenum = GLU_OUTSIDE;
  DrawStyle: TGLenum = GLU_FILL): PGLUQuadric; deprecated 'use TCastleScene to draw 3D stuff';

{ Render sphere in OpenGL. Radius, Slices, Stacks have the same meaning
  as for gluSphere (in case they are not self-explanatory...).
  Other parameters set glu quadric parameters, see glu quadric documentation. }
procedure CastleGluSphere(
  const Radius: TGLdouble;
  const Slices, Stacks: TGLint;
  Texture: boolean = true;
  Normals: TGLenum = GLU_NONE;
  Orientation: TGLenum = GLU_OUTSIDE;
  DrawStyle: TGLenum = GLU_FILL); deprecated 'use TCastleScene to draw 3D stuff';
{$endif}

{ Draw axis (3 lines) around given position.
  Nothing is generated besides vertex positions ---
  no normal vectors, no texture coords, nothing. }
procedure glDrawAxisWire(const Position: TVector3; Size: Single); deprecated 'use TCastleScene to draw 3D stuff';

{ Call glColor, taking Opacity as separate Single argument.
  Deprecated, do not use colors like that, instead pass TCastleColor
  to appropriate routines like TCastleFont.Print.
  @groupBegin }
procedure glColorOpacity(const Color: TVector3; const Opacity: Single); overload; deprecated 'instead of this, use drawing routines that take Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...) or TGLImage.Color';
procedure glColorOpacity(const Color: TVector3Byte; const Opacity: Single); overload; deprecated 'instead of this, use drawing routines that take Color from parameters or properties, like TCastleFont.Print(X,Y,Color,...) or TGLImage.Color';
{ @groupEnd }
{$endif}

{ Utilities for display lists ---------------------------------------- }

{$ifndef OpenGLES}

type
  { }
  EOpenGLNoMoreDisplayLists = class(Exception)
  end;

{ Call glGenLists(range) and checks the result.

  @raises(EOpenGLNoMoreDisplayLists
    When glGenLists(Range) returned zero for non-zero Range.
    The exception's Message shows Place, which may describe
    where this is called --- makes it easier to debug.) }
function glGenListsCheck(range: TGLsizei; const Place: string): TGLuint; deprecated 'do not use display lists; they are not available on OpenGLES and modern OpenGL';

{ If List <> 0 then it does glDeleteList on List and sets List to 0.
  In other words this is simply glDeleteList but
  @orderedList(
    @item only if List really should be deleted
    @item sets List to 0 after deletion
  ) }
procedure glFreeDisplayList(var list: TGLuint); deprecated 'do not use display lists; they are not available on OpenGLES and modern OpenGL';

{$endif}

{ If Buffer <> 0 then it does glDeleteBuffers and sets Buffer to 0. }
procedure glFreeBuffer(var Buffer: TGLuint);

{ Set color and depth buffers writeable or not.
  This is just a shortcut for
  @longcode(#
    glDepthMask(Writeable);
    glColorMask(Writeable, Writeable, Writeable, Writeable);
  #) }
procedure glSetDepthAndColorWriteable(Writeable: TGLboolean);

{ Global position for drawing 2D stuff.
  @deprecated Do this use this.
  Instead of this, use drawing routines that take position as parameters,
  like TGLImageCore.Draw(X,Y) or TCastleFont.Print(X,Y,...). }
var
  WindowPos: TVector2Integer
    deprecated 'use drawing routines that take position as parameters, like TGLImageCore.Draw(X,Y) or TCastleFont.Print(X,Y,...)';

type
  TEnableTextureTarget = (etNone, et2D, etCubeMap, et3D);

{ Enable exactly one (or none, for Target=etNone) OpenGL texture target.
  Always use this instead of manually calling @code(glDisable(GL_TEXTURE_2D)),
  @code(glEnable(GL_TEXTURE_2D)) and such. This makes sure to have at most
  one texture target enabled, and disable others.

  Remember that this state is different for every texture unit in OpenGL,
  in case you use multi-texturing.

  Remember that not all texture targets are guaranteed to be supported by OpenGL.
  Target=etNone and Target=et2D are always supported.
  For the rest, check appropriate GLFeatures property (before even
  creating a texture with such type). If you pass an unsupported target type
  to this procedure, it will be ignored (all targets will be disabled,
  like for Target=etNone).

  Note that this is only for fixed-function OpenGL pipeline.
  Shader pipeline completely ignores the enabled state of texture units. }
function GLEnableTexture(const Target: TEnableTextureTarget): boolean;

{$endif read_interface}

{$ifdef read_implementation}

{ glGetXxx ---------------------------------------------------------------------}

function glGetFloat(pname: TGLEnum): TGLfloat;
begin
  FillChar(result, SizeOf(result), 0);
  glGetFloatv(pname, @result)
end;

function glGetInteger(pname: TGLEnum): TGLint;
begin
  { Just for test, to somewhat simulate hardware with lower
    GL_MAX_TEXTURE_UNITS_ARB,
  if pname = GL_MAX_TEXTURE_UNITS_ARB then
  begin
    Result := 2;
    Exit;
  end;}

  FillChar(result, SizeOf(result), 0);
  glGetIntegerv(pname, @result);
end;

function glGetBoolean(pname: TGLEnum): TGLboolean;
begin
  FillChar(result, SizeOf(result), 0);
  glGetBooleanv(pname, @result)
end;

{ ---------------------------------------------------------------------------- }

{$ifndef OpenGLES}

procedure glColorv(const v: TVector3ub);
begin
  glColor3ubv(@v);
  {$warnings off} // consciously using deprecated in deprecated
  CurrentColor := Vector4(Vector3(V), 1);
  {$warnings on}
end;

procedure glColorv(const v: TVector4ub);
begin
  glColor4ubv(@v);
  {$warnings off} // consciously using deprecated in deprecated
  CurrentColor := Vector4(V);
  {$warnings on}
end;

procedure glColorv(const v: TVector3f);
begin
  glColor3fv(@v);
  {$warnings off} // consciously using deprecated in deprecated
  CurrentColor := Vector4(V, 1);
  {$warnings on}
end;

procedure glColorv(const v: TVector4f);
begin
  glColor4fv(@v);
  {$warnings off} // consciously using deprecated in deprecated
  CurrentColor := V;
  {$warnings on}
end;

procedure glTranslatev(const V: TVector3f); begin glTranslatef(V[0], V[1], V[2]); end;

procedure glTranslatev(const V: TVector3_Single); begin glTranslatef(V.Data[0], V.Data[1], V.Data[2]); end;

procedure glScalev(const V: Single); begin glScalef(V, V, V); end;

procedure glScalev(const V: TVector3f); begin glScalef(V[0], V[1], V[2]); end;

procedure glScalev(const V: TVector3_Single); begin glScalef(V.Data[0], V.Data[1], V.Data[2]); end;

procedure glRotatev(const Angle: TGLfloat;  const V: TVector3f); begin glRotatef(Angle, V[0], V[1], V[2]); end;

procedure glVertexv(const v: TVector2_Single);  begin glVertex2fv(@v.Data); end;

procedure glVertexv(const v: TVector3_Single);  begin glVertex3fv(@v.Data); end;

procedure glVertexv(const v: TVector4_Single);  begin glVertex4fv(@v.Data); end;

procedure glClipPlane(plane: GLenum; const V: TVector4Double);
begin
  GL.glClipPlane(plane, @V);
end;

procedure glNormalv(const v: TVector3); begin glNormal3fv(@v); end;

procedure glMaterialv(face, pname: TGLEnum; const params: TVector4f);  begin glMaterialfv(face, pname, @params); end;

procedure glVertexv(const v: TVector2f);  begin glVertex2fv(@v); end;
procedure glVertexv(const v: TVector2i);  begin glVertex2iv(@v); end;
procedure glVertexv(const v: TVector3f);  begin glVertex3fv(@v); end;
procedure glVertexv(const v: TVector3i);  begin glVertex3iv(@v); end;
procedure glVertexv(const v: TVector4f);  begin glVertex4fv(@v); end;
procedure glVertexv(const v: TVector4i);  begin glVertex4iv(@v); end;

procedure glTexCoordv(const v: TVector2f);  begin glTexCoord2fv(@v); end;
procedure glTexCoordv(const v: TVector3f);  begin glTexCoord3fv(@v); end;
procedure glTexCoordv(const v: TVector4f);  begin glTexCoord4fv(@v); end;

procedure glTexGenv(coord, pname: TGLenum; const params: TVector4f);  begin glTexGenfv(coord, pname, @params); end;

procedure glLightv(light, pname: TGLEnum; const params: TVector4f);  begin glLightfv(light, pname, @params); end;
procedure glLightv(light, pname: TGLEnum; const params: TVector3f);  begin glLightfv(light, pname, @params); end;

procedure glLightModelv(pname: TGLenum; const params: TVector4f); begin glLightModelfv(pname, @params); end;
procedure glLightModelv(pname: TGLenum; const params: TVector4i); begin glLightModeliv(pname, @params); end;

procedure glFogv(pname: TGLEnum; const params: TVector4f);  begin glFogfv(pname, @params); end;

procedure glMultMatrix(const m: TMatrix4f); begin glMultMatrixf(@m) end;
procedure glLoadMatrix(const m: TMatrix4f); begin glLoadMatrixf(@m) end;

procedure glTexEnvv(target, pname: TGLEnum; const params: TVector4f); begin glTexEnvfv(target, pname, @params); end;

{$endif}

procedure GLViewport(const Rect: TRectangle);
begin
  {$ifndef OpenGLES} GL {$else} CastleGLES20 {$endif}
    .glViewport(Rect.Left, Rect.Bottom, Rect.Width, Rect.Height);
end;

{ Various helpers ------------------------------------------------------------ }

procedure GLSetEnabled(value: TGLenum; isEnabled: boolean);
begin
  if isEnabled then glEnable(value) else glDisable(value);
end;

{$ifndef OpenGLES}
procedure GLVerticalLine(x, y1, y2: TGLfloat);
begin
  glBegin(GL_LINES); glVertex2f(x, y1); glVertex2f(x, y2); glEnd;
end;

procedure GLHorizontalLine(x1, x2, y: TGLfloat);
begin
  glBegin(GL_LINES); glVertex2f(x1, y); glVertex2f(x2, y); glEnd;
end;

procedure GLDrawArrow(HeadThickness, HeadLength: TGLfloat);
begin
  HeadLength := 2*HeadLength; { mapuj HeadLength na zakres 0..2 }

  { TODO: tutaj powinienes przelaczac glEdgeFlag }

  glBegin(GL_TRIANGLES);
    glVertex2f(0, 2);
    glVertex2f(-1, HeadLength);
    glVertex2f(-HeadThickness, HeadLength);

    glVertex2f(0, 2);
    glVertex2f(-HeadThickness, HeadLength);
    glVertex2f(HeadThickness, HeadLength);

    glVertex2f(0, 2);
    glVertex2f(HeadThickness, HeadLength);
    glVertex2f(1, HeadLength);
  glEnd;

  glBegin(GL_QUADS);
    glVertex2f(-HeadThickness, HeadLength);
    glVertex2f(-HeadThickness, 0);
    glVertex2f(HeadThickness, 0);
    glVertex2f(HeadThickness, HeadLength);
  glEnd;
end;

(* // These versions would work with OpenGLES. But they ignore OpenGL matrix state like glTransform etc.

procedure GLVerticalLine(x, y1, y2: TGLfloat);
begin
  DrawPrimitive2D(pmLines,
    [Vector2SmallInt(Round(x), Round(y1)),
     Vector2SmallInt(Round(x), Round(y2))],
    CurrentColor);
end;

procedure GLHorizontalLine(x1, x2, y: TGLfloat);
begin
  DrawPrimitive2D(pmLines,
    [Vector2SmallInt(Round(x1), Round(y)),
     Vector2SmallInt(Round(x2), Round(y))],
    CurrentColor);
end;

procedure GLDrawArrow(HeadThickness, HeadLength: TGLfloat);
begin
  HeadLength := 2*HeadLength; { mapuj HeadLength na zakres 0..2 }

  DrawPrimitive2D(pmTriangles,
    [Vector2SmallInt(0, 2),
     Vector2SmallInt(-1, Round(HeadLength)),
     Vector2SmallInt(-Round(HeadThickness), Round(HeadLength)),

     Vector2SmallInt(0, 2),
     Vector2SmallInt(-Round(HeadThickness), Round(HeadLength)),
     Vector2SmallInt(Round(HeadThickness), Round(HeadLength)),

     Vector2SmallInt(0, 2),
     Vector2SmallInt(Round(HeadThickness), Round(HeadLength)),
     Vector2SmallInt(1, Round(HeadLength)),

     // quad
     Vector2SmallInt(-Round(HeadThickness), Round(HeadLength)),
     Vector2SmallInt(-Round(HeadThickness), 0),
     Vector2SmallInt(Round(HeadThickness), 0),

     Vector2SmallInt(-Round(HeadThickness), Round(HeadLength)),
     Vector2SmallInt(Round(HeadThickness), 0),
     Vector2SmallInt(Round(HeadThickness), Round(HeadLength))
    ],
    CurrentColor);
end;
*)

{$ifdef CASTLE_OBJFPC}
function NewGLUQuadric(texture: boolean; normals: TGLenum;
  orientation: TGLenum; drawStyle: TGLenum): PGLUQuadric;
begin
  result := gluNewQuadric();
  if result = nil then
    raise Exception.Create('gluNewQuadric cannot be created');
  {$push}
  {$warnings off}
  // knowingly using deprecated in another deprecated
  gluQuadricCallback(result, GLU_ERROR, TCallBack(@GLErrorRaise));
  {$pop}
  gluQuadricTexture(result, Ord(texture));
  gluQuadricNormals(result, normals);
  gluQuadricOrientation(result, orientation);
  gluQuadricDrawStyle(result, drawStyle);
end;

procedure CastleGluSphere(
  const Radius: TGLdouble;
  const Slices, Stacks: TGLint;
  Texture: boolean; Normals: TGLenum;
  Orientation: TGLenum; DrawStyle: TGLenum);
var
  Q: PGLUQuadric;
begin
  {$warnings off} { deliberately using deprecated stuff inside another deprecated }
  Q := NewGLUQuadric(Texture, Normals, Orientation, DrawStyle);
  {$warnings on}
  try
    gluSphere(Q, Radius, Slices, Stacks);
  finally gluDeleteQuadric(Q); end;
end;
{$endif CASTLE_OBJFPC}

procedure glDrawAxisWire(const Position: TVector3; Size: Single);
begin
  Size := Size / 2;
  glBegin(GL_LINES);
    glVertexv(Position - Vector3(Size, 0, 0));
    glVertexv(Position + Vector3(Size, 0, 0));
    glVertexv(Position - Vector3(0, Size, 0));
    glVertexv(Position + Vector3(0, Size, 0));
    glVertexv(Position - Vector3(0, 0, Size));
    glVertexv(Position + Vector3(0, 0, Size));
  glEnd;
end;

procedure glColorOpacity(const Color: TVector3; const Opacity: Single);
begin
  glColor4f(Color[0], Color[1], Color[2], Opacity);
end;

procedure glColorOpacity(const Color: TVector3Byte; const Opacity: Single);
begin
  glColor4f(Color[0] / 255, Color[1] / 255, Color[2] / 255, Opacity);
end;
{$endif}

{$ifndef OpenGLES}

function glGenListsCheck(range: TGLsizei; const Place: string): TGLuint;
begin
  Result := glGenLists(range);
  if (Result = 0) and (range <> 0) then
    raise EOpenGLNoMoreDisplayLists.CreateFmt(
      'No more OpenGL display lists available when trying to allocate new %d display lists from "%s". This may mean that GPU memory is full (low possibility, unless you used some ridiculous number of display lists), or that OpenGL context is not initialized yet', [range, Place]);
end;

procedure glFreeDisplayList(var list: TGLuint);
begin
 if list <> 0 then
 begin
  glDeleteLists(list, 1);
  list := 0;
 end;
end;

{$endif}

procedure glFreeBuffer(var Buffer: TGLuint);
begin
  if Buffer <> 0 then
  begin
    glDeleteBuffers(1, @Buffer);
    Buffer := 0;
  end;
end;

procedure glSetDepthAndColorWriteable(Writeable: TGLboolean);
begin
  glDepthMask(Writeable);
  glColorMask(Writeable, Writeable, Writeable, Writeable);
end;

function GLEnableTexture(const Target: TEnableTextureTarget): boolean;
begin
  Result := true;

  {$ifndef OpenGLES}

  case Target of
    etNone: begin
        glDisable(GL_TEXTURE_2D);
        if GLFeatures.TextureCubeMap <> gsNone then glDisable(GL_TEXTURE_CUBE_MAP);
        if GLFeatures.Texture3D <> gsNone then glDisable(GL_TEXTURE_3D);
      end;
    et2D: begin
        glEnable(GL_TEXTURE_2D);
        if GLFeatures.TextureCubeMap <> gsNone then glDisable(GL_TEXTURE_CUBE_MAP);
        if GLFeatures.Texture3D <> gsNone then glDisable(GL_TEXTURE_3D);
      end;
    etCubeMap: begin
        glDisable(GL_TEXTURE_2D);
        if GLFeatures.TextureCubeMap <> gsNone then glEnable(GL_TEXTURE_CUBE_MAP) else Result := false;
        if GLFeatures.Texture3D <> gsNone then glDisable(GL_TEXTURE_3D);
      end;
    et3D: begin
        glDisable(GL_TEXTURE_2D);
        if GLFeatures.TextureCubeMap <> gsNone then glDisable(GL_TEXTURE_CUBE_MAP);
        if GLFeatures.Texture3D <> gsNone then glEnable(GL_TEXTURE_3D) else Result := false;
      end;
    else raise EInternalError.Create('GLEnableTexture:Target?');
  end;

  {$endif}
end;

{$endif read_implementation}
