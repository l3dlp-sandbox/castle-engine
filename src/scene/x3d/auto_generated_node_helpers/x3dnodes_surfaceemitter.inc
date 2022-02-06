{ -*- buffer-read-only: t -*-

  Copyright 2015-2021 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Automatically generated node properties.

  Do not edit this file manually!
  To add new properties:
  - add them to the text files in tools/internal/x3d-nodes-to-pascal/nodes-specification/ ,
  - and regenerate include files by running x3d-nodes-to-pascal .

  The documentation for properties should go to x3dnodes_documentation.txt . }

{$ifdef read_interface}

  strict private
    function GetSurface: TAbstractGeometryNode;
    procedure SetSurface(const Value: TAbstractGeometryNode);
  public
    procedure CreateNode; override;
    class function ClassX3DType: string; override;

    strict private FEventSet_coordIndex: TMFInt32Event;
    { X3D in event "set_coordIndex". } { }
    public property EventSet_coordIndex: TMFInt32Event read FEventSet_coordIndex;

    strict private FFdCoordIndex: TMFInt32;
    { X3D field "coordIndex". } { }
    public property FdCoordIndex: TMFInt32 read FFdCoordIndex;
    { Set X3D field "coordIndex". } { }
    procedure SetCoordIndex(const Value: array of LongInt); overload;
    { Set X3D field "coordIndex". } { }
    procedure SetCoordIndex(const Value: TLongIntList); overload;

    strict private FFdSurface: TSFNode;
    { X3D field "surface". } { }
    public property FdSurface: TSFNode read FFdSurface;
    { X3D field "surface". } { }
    property Surface: TAbstractGeometryNode read GetSurface write SetSurface;

{$endif read_interface}

{$ifdef read_implementation}

{ TSurfaceEmitterNode ----------------------------------------------- }

procedure TSurfaceEmitterNode.SetCoordIndex(const Value: array of LongInt);
begin
  FdCoordIndex.Send(Value);
end;

procedure TSurfaceEmitterNode.SetCoordIndex(const Value: TLongIntList);
begin
  FdCoordIndex.Send(Value);
end;

function TSurfaceEmitterNode.GetSurface: TAbstractGeometryNode;
begin
  if FdSurface.Value is TAbstractGeometryNode then
    Result := TAbstractGeometryNode(FdSurface.Value)
  else
    Result := nil;
end;

procedure TSurfaceEmitterNode.SetSurface(const Value: TAbstractGeometryNode);
begin
  FdSurface.Send(Value);
end;

class function TSurfaceEmitterNode.ClassX3DType: String;
begin
  Result := 'SurfaceEmitter';
end;

procedure TSurfaceEmitterNode.CreateNode;
begin
  inherited;

  FEventSet_coordIndex := TMFInt32Event.Create(Self, 'set_coordIndex', true);
  AddEvent(FEventSet_coordIndex);

  FFdCoordIndex := TMFInt32.Create(Self, False, 'coordIndex', [ -1]);
   FdCoordIndex.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdCoordIndex);

  FFdSurface := TSFNode.Create(Self, False, 'surface', [TAbstractGeometryNode]);
   FdSurface.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdSurface);
end;

{$endif read_implementation}