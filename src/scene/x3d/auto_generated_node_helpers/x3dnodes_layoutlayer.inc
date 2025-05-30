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
    function GetLayout: TAbstractLayoutNode;
    procedure SetLayout(const Value: TAbstractLayoutNode);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FEventAddChildren: TMFNodeEvent;
    {  }
    public property EventAddChildren: TMFNodeEvent read FEventAddChildren;

    strict private FEventRemoveChildren: TMFNodeEvent;
    {  }
    public property EventRemoveChildren: TMFNodeEvent read FEventRemoveChildren;

    strict private FFdChildren: TMFNode;
    { Internal wrapper for property @code(Children). This wrapper API may change, we advise to access simpler @code(Children) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdChildren: TMFNode read FFdChildren;
    {  }
    procedure SetChildren(const Value: array of TAbstractChildNode);

    strict private FFdLayout: TSFNode;
    { Internal wrapper for property @code(Layout). This wrapper API may change, we advise to access simpler @code(Layout) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdLayout: TSFNode read FFdLayout;
    {  }
    property Layout: TAbstractLayoutNode read GetLayout write SetLayout;

{$endif read_interface}

{$ifdef read_implementation}

{ TLayoutLayerNode ----------------------------------------------- }

procedure TLayoutLayerNode.SetChildren(const Value: array of TAbstractChildNode);
var
  L: Integer;
  A: array of TX3DNode;
begin
  L := High(Value) + 1;
  SetLength(A, L);
  if L > 0 then
    Move(Value[0], A[0], L * SizeOf(TAbstractChildNode));
  FdChildren.Send(A);
end;

function TLayoutLayerNode.GetLayout: TAbstractLayoutNode;
begin
  if FdLayout.Value is TAbstractLayoutNode then
    Result := TAbstractLayoutNode(FdLayout.Value)
  else
    Result := nil;
end;

procedure TLayoutLayerNode.SetLayout(const Value: TAbstractLayoutNode);
begin
  FdLayout.Send(Value);
end;

class function TLayoutLayerNode.ClassX3DType: String;
begin
  Result := 'LayoutLayer';
end;

procedure TLayoutLayerNode.CreateNode;
begin
  inherited;

  FEventAddChildren := TMFNodeEvent.Create(Self, 'addChildren', true);
  AddEvent(FEventAddChildren);

  FEventRemoveChildren := TMFNodeEvent.Create(Self, 'removeChildren', true);
  AddEvent(FEventRemoveChildren);

  FFdChildren := TMFNode.Create(Self, True, 'children', [TAbstractChildNode]);
   FdChildren.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdChildren);

  FFdLayout := TSFNode.Create(Self, True, 'layout', [TAbstractLayoutNode]);
   FdLayout.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdLayout);
end;

{$endif read_implementation}
