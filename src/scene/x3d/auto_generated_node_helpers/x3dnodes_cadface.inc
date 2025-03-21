{ -*- buffer-read-only: t -*-

  Copyright 2015-2025 Michalis Kamburelis.

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
  - and regenerate include files by running x3d-nodes-to-pascal }

{$ifdef read_interface}

  strict private
    function GetShape: TX3DNode;
    procedure SetShape(const Value: TX3DNode);
    function GetBBox: TBox3D;
    procedure SetBBox(const Value: TBox3D);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FFdShape: TSFNode;
    { Internal wrapper for property @code(Shape). This wrapper API may change, we advise to access simpler @code(Shape) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdShape: TSFNode read FFdShape;
    {  }
    property Shape: TX3DNode read GetShape write SetShape;

    strict private FFdBboxCenter: TSFVec3f;
    { Internal wrapper for property @code(BboxCenter). This wrapper API may change, we advise to access simpler @code(BboxCenter) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdBboxCenter: TSFVec3f read FFdBboxCenter;
    {  }
    property BBox: TBox3D read GetBBox write SetBBox;

    strict private FFdBboxSize: TSFVec3f;
    { Internal wrapper for property @code(BboxSize). This wrapper API may change, we advise to access simpler @code(BboxSize) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdBboxSize: TSFVec3f read FFdBboxSize;

{$endif read_interface}

{$ifdef read_implementation}

{ TCADFaceNode ----------------------------------------------- }

function TCADFaceNode.GetShape: TX3DNode;
begin
  if FdShape.Value is TX3DNode then
    Result := TX3DNode(FdShape.Value)
  else
    Result := nil;
end;

procedure TCADFaceNode.SetShape(const Value: TX3DNode);
begin
  FdShape.Send(Value);
end;

function TCADFaceNode.GetBBox: TBox3D;
begin
  Result := TBox3D.FromCenterSize(FdBBoxCenter.Value, FdBBoxSize.Value);
end;

procedure TCADFaceNode.SetBBox(const Value: TBox3D);
var
  ValueCenter, ValueSize: TVector3;
begin
  Value.ToCenterSize(ValueCenter, ValueSize);
  FdBBoxCenter.Send(ValueCenter);
  FdBBoxSize.Send(ValueSize);
end;

class function TCADFaceNode.ClassX3DType: String;
begin
  Result := 'CADFace';
end;

procedure TCADFaceNode.CreateNode;
begin
  inherited;

  FFdShape := TSFNode.Create(Self, True, 'shape', [TAbstractShapeNode, TLODNode, TTransformNode]);
   FdShape.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdShape);

  FFdBboxCenter := TSFVec3f.Create(Self, False, 'bboxCenter', Vector3(0, 0, 0));
   FdBboxCenter.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdBboxCenter);

  FFdBboxSize := TSFVec3f.Create(Self, False, 'bboxSize', Vector3(-1, -1, -1));
   FdBboxSize.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdBboxSize);
end;

{$endif read_implementation}
