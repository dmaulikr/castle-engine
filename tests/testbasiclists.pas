{
  Copyright 2004-2010 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

unit TestBasicLists;

interface

uses
  Classes, SysUtils, fpcunit, testutils, testregistry;

type
  TTestBasicLists = class(TTestCase)
    procedure TestSetCountItems;
    procedure TestPrimitiveLists;
    procedure TestVectorMathLists;
    procedure TestListsAssign;
    procedure TestListsAssignLerp;
    procedure TestZero;
  end;

implementation

uses KambiUtils, VectorMath;

function IsSmallerString(const a, b: string): Integer;
begin
  if A < B then
    Result := -1 else
  if A > B then
    Result :=  1 else
    Result :=  0;
end;

procedure TTestBasicLists.TestSetCountItems;
var
  iarr: TIntegerList;
begin
  iarr := TIntegerList.Create;
  try
    Assert(iarr.Count = 0);

    { growing count works, and sets Items to non-nil }
    iarr.Count := 3;
    Assert(iarr.Count = 3);
    Assert(iarr.List <> nil);

    { growing count doesn't change previous values }
    iarr[2] := 123;
    iarr.Count := 1000000;
    Assert(iarr[2] = 123);
  finally FreeAndNil(iarr) end;
end;

(*
procedure TTestBasicLists.TestGenericStringList;

  function Equal(const S1: TGenericStringList; const S2: array of string): boolean;
  var
    I: Integer;
  begin
    Result := S1.Count = High(S2) + 1;
    if Result then
    begin
      for I := 0 to S1.Count - 1 do
        if S1[I] <> S2[I] then
          Exit(false);
      Result := true;
    end;
  end;

  procedure Reverse(S: TGenericStringList);
  var
    I: Integer;
  begin
    { Need to specially check for Count = 0 case, since (0-1) div 2 = -1 div 2 = 0
      which means that loop would try invalid Exchange(0, -1). }
    if S.Count = 0 then Exit;
    for I := 0 to (S.Count - 1) div 2 do
      S.Exchange(I, S.Count - 1 - I);
  end;

var
  sarr, sarr2: TGenericStringList;
  i, j: integer;
const
  twoStrings: array[0..1]of string = ('raz','dwa');
begin
 for i := 1 to 100 do
 begin
  sarr := TGenericStringList.Create;
  try
   sarr.Capacity := Random(8);
   sarr.Count := 4;
   sarr[0] := 'FOO';
   sarr[1] := 'foo bar xyz';
   sarr.Delete(0);
   sarr.AddArray(twoStrings);
   sarr.Add('trzy?');

   Assert(not Equal(sarr, ['foo bar xyz', '', '']));
   Assert(Equal(sarr, ['foo bar xyz', '', '', 'raz', 'dwa', 'trzy?']));

   Reverse(sarr);
   Assert(Equal(sarr, ['trzy?', 'dwa', 'raz', '', '', 'foo bar xyz']));

   sarr2 := TGenericStringList.Create;
   try
    sarr2.Add('blah');
    Assert(Equal(sarr2, ['blah']));
    sarr2.Assign(sarr);
    Assert(Equal(sarr2, ['trzy?', 'dwa', 'raz', '', '', 'foo bar xyz']));

    {sortuj ustalone 6 stringow}
    sarr.Sort(@IsSmallerString);
    Assert(Equal(sarr, ['', '', 'dwa', 'foo bar xyz', 'raz', 'trzy?']));

    { sprawdz ze kolejnosc na sarr2 pozostala niezmieniona }
    Assert(Equal(sarr2, ['trzy?', 'dwa', 'raz', '', '', 'foo bar xyz']));
   finally sarr2.Free end;

   {dodaj losowe stringi, sortuj, sprawdz}
   for j := 0 to 20 do
    sarr.Add( Chr(Random(256)) + Chr(Random(256)) + Chr(Random(256)) );
   sarr.Sort(@IsSmallerString);
   for j := 0 to sarr.Count-2 do Assert(sarr[j] <= sarr[j+1]);

  finally sarr.Free end;
 end;

 sarr := TGenericStringList.Create;
 try
  { na tablicy o 0 liczbie elementow tez wszystko powinno isc ok }
  Assert(sarr.Count = 0);
  Reverse(sarr);
  Assert(sarr.Count = 0);
 finally sarr.Free end;
end;
*)

procedure TTestBasicLists.TestPrimitiveLists;

  function Equal(const S1: TIntegerList; const S2: array of Integer): boolean;
  var
    I: Integer;
  begin
    Result := S1.Count = High(S2) + 1;
    if Result then
    begin
      for I := 0 to S1.Count - 1 do
        if S1[I] <> S2[I] then
          Exit(false);
      Result := true;
    end;
  end;

  function Equal(const S1, S2: TIntegerList): boolean;
  var
    I: Integer;
  begin
    Result := S1.Count = S2.Count;
    if Result then
    begin
      for I := 0 to S1.Count - 1 do
        if S1[I] <> S2[I] then
          Exit(false);
      Result := true;
    end;
  end;

var
  iarr, iarr2: TIntegerList;
begin
 iarr := nil;
 iarr2 := nil;
 try
  iarr := TIntegerList.Create;
  iarr2 := TIntegerList.Create;
  Assert(Equal(iarr, iarr2));
  Assert(iarr.Sum = 0);
  Assert(iarr2.Sum = 0);

  iarr.AddArray([1, 3, 6, 8]);
  Assert(not Equal(iarr, iarr2));
  Assert(iarr.Sum = 1 + 3 + 6 + 8);
  iarr2.AddArray([1, 3, 6, 8]);
  Assert(Equal(iarr, iarr2));
  iarr2.Insert(0, 99);
  Assert(not Equal(iarr, iarr2));
  Assert(iarr2.Sum = iarr.Sum + 99);
  Assert(Equal(iarr2, [99, 1, 3, 6, 8]));

  iarr.Assign(iarr2);
  Assert(Equal(iarr, iarr2));
  Assert(Equal(iarr, [99, 1, 3, 6, 8]));

  { simple DeleteDuplicates tests }

  { DeleteDuplicates removed, since not used, and not avail in FGL.

procedure DeleteDuplicates;
var
  I, Index: integer;
begin
  I := 0;
  while I < Count do
  begin
    Index := I + 1;
    repeat
      Index := IndexOf(Items[I], Index);
      if Index = -1 then Break;
      Delete(Index);
    until false;

    Inc(I);
  end;
end;
}

{  IArr.DeleteDuplicates;
  Assert(iarr.Equal([99, 1, 3, 6, 8]));
  IArr.Insert(0, 3);
  IArr.DeleteDuplicates;
  Assert(iarr.Equal([3, 99, 1, 6, 8]));
  IArr.Count := 0;
  IArr.DeleteDuplicates;
  Assert(iarr.Equal([]));}
 finally
  iarr.Free;
  iarr2.Free;
 end;
end;

procedure TTestBasicLists.TestVectorMathLists;
var
  vecs: TVector3SingleList;
begin
  vecs := TVector3SingleList.Create;
  try
    vecs.Add(Vector3Single(1.0, 2.0, 3.0));
    vecs.Add(Vector3Single(4.0, 5.0, 6.0));
    vecs.Add(Vector3Single(1.0, 2.0, 3.0));
    Assert(    VectorsPerfectlyEqual(vecs.L[0], vecs.L[2]));
    Assert(not VectorsPerfectlyEqual(vecs.L[0], vecs.L[1]));
    Assert(not VectorsPerfectlyEqual(vecs.L[2], vecs.L[1]));
  finally FreeAndNil(vecs) end;
end;

procedure TTestBasicLists.TestListsAssign;
var
  V1, V2: TVector3SingleList;
begin
  V1 := TVector3SingleList.Create;
  V2 := TVector3SingleList.Create;
  try
    V1.Add(Vector3Single(1.0, 2.0, 3.0));
    V1.Add(Vector3Single(4.0, 5.0, 6.0));
    V1.Add(Vector3Single(7.0, 8.0, 9.0));

    V2.Add(Vector3Single(6.0, 6.0, 6.0));
    V2.AddList(V1);
    V2.Add(Vector3Single(6.0, 6.0, 6.0));

    Assert(VectorsPerfectlyEqual(V1.L[0], V2.L[1]));
    Assert(VectorsPerfectlyEqual(V1.L[1], V2.L[2]));
    Assert(VectorsPerfectlyEqual(V1.L[2], V2.L[3]));

    V2.AddListRange(V1, 1, 1);
    Assert(V2.Count = 6);
    Assert(VectorsPerfectlyEqual(V1.L[1], V2.L[5]));
  finally
    FreeAndNil(V1);
    FreeAndNil(V2);
  end;
end;

procedure TTestBasicLists.TestListsAssignLerp;
var
  V1, V2, V3: TVector3SingleList;
begin
  V1 := TVector3SingleList.Create;
  V2 := TVector3SingleList.Create;
  V3 := TVector3SingleList.Create;
  try
    V1.Add(Vector3Single(1.0, 2.0, 3.0));
    V1.Add(Vector3Single(4.0, 5.0, 6.0));

    V2.Add(Vector3Single(7.0, 8.0, 9.0));
    V2.Add(Vector3Single(11.0, 12.0, 13.0));
    V2.Add(Vector3Single(17.0, 18.0, 19.0));

    V3.AssignLerp(0.2, V1, V2, 0, 1, 2);
    Assert(V3.Count = 2);

    Assert(VectorsPerfectlyEqual(V3.L[0], Lerp(0.2,
      Vector3Single(1.0, 2.0, 3.0),
      Vector3Single(11.0, 12.0, 13.0))));
    Assert(VectorsPerfectlyEqual(V3.L[1], Lerp(0.2,
      Vector3Single(4.0, 5.0, 6.0),
      Vector3Single(17.0, 18.0, 19.0))));
  finally
    FreeAndNil(V1);
    FreeAndNil(V2);
    FreeAndNil(V3);
  end;
end;

procedure TTestBasicLists.TestZero;
var
  B: TBooleanList;
  O: TIntegerList;
  F: TSingleList;
  I: Integer;
begin
  { For lists based on TFPGList, increasing count always initializes new memory
    to zero. }

  B := TBooleanList.Create;
  B.Count := 10;
  for I := 0 to B.Count - 1 do
    Assert(not B[I]);
  FreeAndNil(B);

  O := TIntegerList.Create;
  O.Count := 10;
  for I := 0 to O.Count - 1 do
    Assert(O[I] = 0);
  FreeAndNil(O);

  F := TSingleList.Create;
  F.Count := 10;
  for I := 0 to F.Count - 1 do
    Assert(F[I] = 0);
  FreeAndNil(F);
end;

initialization
 RegisterTest(TTestBasicLists);
end.
