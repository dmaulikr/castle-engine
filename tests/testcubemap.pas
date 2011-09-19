{
  Copyright 2008-2010 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

unit TestCubeMap;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, fpcunit, testutils, testregistry;

type
  TTestCubeMap = class(TTestCase)
  published
    procedure TestReverse;
    procedure TestCubeMapSolidAngle;
  end;

implementation

uses VectorMath, CubeMap, Math;

procedure TTestCubeMap.TestReverse;
var
  Side, NewSide: TCubeMapSide;
  Pixel, NewPixel: Cardinal;
  Dir: TVector3Single;
begin
  for Side := Low(Side) to High(Side) do
    for Pixel := 0 to Sqr(CubeMapSize) - 1 do
    begin
      Dir := CubeMapDirection(Side, Pixel);
      DirectionToCubeMap(Dir, NewSide, NewPixel);
      Assert(NewSide = Side);
      Assert(NewPixel = Pixel);
    end;
end;

type
  generic TTester<T> = class
    procedure DoTest;
  end;
  TTesterSingle = specialize TTester<Single>;
  TTesterDouble = specialize TTester<Double>;
  TTesterExtended = specialize TTester<Extended>;

procedure TTester.DoTest;
var
  Side: TCubeMapSide;
  Pixel: Cardinal;
  SphereArea, SphereArea2: T;
begin
  SphereArea := 0;
  SphereArea2 := 0;

  for Side := Low(Side) to High(Side) do
    for Pixel := 0 to Sqr(CubeMapSize) - 1 do
    begin
      SphereArea += CubeMapSolidAngle(Side, Pixel);
      SphereArea2 += (4 * Pi)/(6*Sqr(CubeMapSize));
    end;

  { I use SphereArea2 only to test the accuracy of addition:
    remember that even if CubeMapSolidAngle would be perfect,
    adding "6*Sqr(CubeMapSize)" values together always has some precision
    error accumulated.

    Results comparing SphereArea2 with (4 * Pi) (accuracy of addition):

    Single:
      12.566516876220703
      12.566370614359173
            | - difference here

    Double:
      12.566370614359418
      12.566370614359173
                     | - difference here

    Extended:
      12.566370614359173
      12.566370614359173

                         | no visible difference of these digits.
                           Looked at 30 first decimal digits,
                           still no difference.
  }

{
  Writeln(Format('%g', [SphereArea]));
//  Writeln(Format('%g', [SphereArea2]));
  Writeln(Format('%g', [4 * Pi]));
}

  { CubeMapSolidAngle is a gross approximation now, so we allow quite large
    error. }
  Assert(FloatsEqual(SphereArea, 4 * Pi, 0.02));
end;

procedure TTestCubeMap.TestCubeMapSolidAngle;
var
  TS: TTesterSingle;
  TD: TTesterDouble;
  TE: TTesterExtended;
begin
  TS := TTesterSingle.Create;
  TS.DoTest;
  TS.Free;

  TD := TTesterDouble.Create;
  TD.DoTest;
  TD.Free;

  TE := TTesterExtended.Create;
  TE.DoTest;
  TE.Free;
end;

initialization
  RegisterTest(TTestCubeMap);
end.
