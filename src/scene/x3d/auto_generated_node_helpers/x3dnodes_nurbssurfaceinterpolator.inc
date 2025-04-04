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
    function GetControlPoint: TAbstractCoordinateNode;
    procedure SetControlPoint(const Value: TAbstractCoordinateNode);
    function GetUDimension: Integer;
    procedure SetUDimension(const Value: Integer);
    function GetUOrder: Integer;
    procedure SetUOrder(const Value: Integer);
    function GetVDimension: Integer;
    procedure SetVDimension(const Value: Integer);
    function GetVOrder: Integer;
    procedure SetVOrder(const Value: Integer);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FEventSet_fraction: TSFVec2fEvent;
    {  }
    public property EventSet_fraction: TSFVec2fEvent read FEventSet_fraction;

    strict private FFdControlPoint: TSFNode;
    { Internal wrapper for property @code(ControlPoint). This wrapper API may change, we advise to access simpler @code(ControlPoint) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdControlPoint: TSFNode read FFdControlPoint;
    {  }
    property ControlPoint: TAbstractCoordinateNode read GetControlPoint write SetControlPoint;

    strict private FFdWeight: TMFDouble;
    { Internal wrapper for property @code(Weight). This wrapper API may change, we advise to access simpler @code(Weight) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdWeight: TMFDouble read FFdWeight;
    {  }
    procedure SetWeight(const Value: array of Double); overload;
    {  }
    procedure SetWeight(const Value: TDoubleList); overload;

    strict private FEventPosition_changed: TSFVec3fEvent;
    {  }
    public property EventPosition_changed: TSFVec3fEvent read FEventPosition_changed;

    strict private FEventNormal_changed: TSFVec3fEvent;
    {  }
    public property EventNormal_changed: TSFVec3fEvent read FEventNormal_changed;

    strict private FFdUDimension: TSFInt32;
    { Internal wrapper for property @code(UDimension). This wrapper API may change, we advise to access simpler @code(UDimension) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdUDimension: TSFInt32 read FFdUDimension;
    {  }
    property UDimension: Integer read GetUDimension write SetUDimension;

    strict private FFdUKnot: TMFDouble;
    { Internal wrapper for property @code(UKnot). This wrapper API may change, we advise to access simpler @code(UKnot) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdUKnot: TMFDouble read FFdUKnot;
    {  }
    procedure SetUKnot(const Value: array of Double); overload;
    {  }
    procedure SetUKnot(const Value: TDoubleList); overload;

    strict private FFdUOrder: TSFInt32;
    { Internal wrapper for property @code(UOrder). This wrapper API may change, we advise to access simpler @code(UOrder) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdUOrder: TSFInt32 read FFdUOrder;
    {  }
    property UOrder: Integer read GetUOrder write SetUOrder;

    strict private FFdVDimension: TSFInt32;
    { Internal wrapper for property @code(VDimension). This wrapper API may change, we advise to access simpler @code(VDimension) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdVDimension: TSFInt32 read FFdVDimension;
    {  }
    property VDimension: Integer read GetVDimension write SetVDimension;

    strict private FFdVKnot: TMFDouble;
    { Internal wrapper for property @code(VKnot). This wrapper API may change, we advise to access simpler @code(VKnot) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdVKnot: TMFDouble read FFdVKnot;
    {  }
    procedure SetVKnot(const Value: array of Double); overload;
    {  }
    procedure SetVKnot(const Value: TDoubleList); overload;

    strict private FFdVOrder: TSFInt32;
    { Internal wrapper for property @code(VOrder). This wrapper API may change, we advise to access simpler @code(VOrder) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdVOrder: TSFInt32 read FFdVOrder;
    {  }
    property VOrder: Integer read GetVOrder write SetVOrder;

{$endif read_interface}

{$ifdef read_implementation}

{ TNurbsSurfaceInterpolatorNode ----------------------------------------------- }

function TNurbsSurfaceInterpolatorNode.GetControlPoint: TAbstractCoordinateNode;
begin
  if FdControlPoint.Value is TAbstractCoordinateNode then
    Result := TAbstractCoordinateNode(FdControlPoint.Value)
  else
    Result := nil;
end;

procedure TNurbsSurfaceInterpolatorNode.SetControlPoint(const Value: TAbstractCoordinateNode);
begin
  FdControlPoint.Send(Value);
end;

procedure TNurbsSurfaceInterpolatorNode.SetWeight(const Value: array of Double);
begin
  FdWeight.Send(Value);
end;

procedure TNurbsSurfaceInterpolatorNode.SetWeight(const Value: TDoubleList);
begin
  FdWeight.Send(Value);
end;

function TNurbsSurfaceInterpolatorNode.GetUDimension: Integer;
begin
  Result := FdUDimension.Value;
end;

procedure TNurbsSurfaceInterpolatorNode.SetUDimension(const Value: Integer);
begin
  FdUDimension.Send(Value);
end;

procedure TNurbsSurfaceInterpolatorNode.SetUKnot(const Value: array of Double);
begin
  FdUKnot.Send(Value);
end;

procedure TNurbsSurfaceInterpolatorNode.SetUKnot(const Value: TDoubleList);
begin
  FdUKnot.Send(Value);
end;

function TNurbsSurfaceInterpolatorNode.GetUOrder: Integer;
begin
  Result := FdUOrder.Value;
end;

procedure TNurbsSurfaceInterpolatorNode.SetUOrder(const Value: Integer);
begin
  FdUOrder.Send(Value);
end;

function TNurbsSurfaceInterpolatorNode.GetVDimension: Integer;
begin
  Result := FdVDimension.Value;
end;

procedure TNurbsSurfaceInterpolatorNode.SetVDimension(const Value: Integer);
begin
  FdVDimension.Send(Value);
end;

procedure TNurbsSurfaceInterpolatorNode.SetVKnot(const Value: array of Double);
begin
  FdVKnot.Send(Value);
end;

procedure TNurbsSurfaceInterpolatorNode.SetVKnot(const Value: TDoubleList);
begin
  FdVKnot.Send(Value);
end;

function TNurbsSurfaceInterpolatorNode.GetVOrder: Integer;
begin
  Result := FdVOrder.Value;
end;

procedure TNurbsSurfaceInterpolatorNode.SetVOrder(const Value: Integer);
begin
  FdVOrder.Send(Value);
end;

class function TNurbsSurfaceInterpolatorNode.ClassX3DType: String;
begin
  Result := 'NurbsSurfaceInterpolator';
end;

procedure TNurbsSurfaceInterpolatorNode.CreateNode;
begin
  inherited;

  FEventSet_fraction := TSFVec2fEvent.Create(Self, 'set_fraction', true);
  AddEvent(FEventSet_fraction);

  FFdControlPoint := TSFNode.Create(Self, True, 'controlPoint', [TAbstractCoordinateNode]);
   FdControlPoint.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdControlPoint);

  FFdWeight := TMFDouble.Create(Self, True, 'weight', []);
   FdWeight.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdWeight);

  FEventPosition_changed := TSFVec3fEvent.Create(Self, 'position_changed', false);
  AddEvent(FEventPosition_changed);

  FEventNormal_changed := TSFVec3fEvent.Create(Self, 'normal_changed', false);
  AddEvent(FEventNormal_changed);

  FFdUDimension := TSFInt32.Create(Self, False, 'uDimension', 0);
   FdUDimension.ChangeAlways := chVisibleNonGeometry;
   FdUDimension.MustBeNonnegative := true;
  AddField(FFdUDimension);

  FFdUKnot := TMFDouble.Create(Self, False, 'uKnot', []);
   FdUKnot.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdUKnot);

  FFdUOrder := TSFInt32.Create(Self, False, 'uOrder', 3);
   FdUOrder.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdUOrder);

  FFdVDimension := TSFInt32.Create(Self, False, 'vDimension', 0);
   FdVDimension.ChangeAlways := chVisibleNonGeometry;
   FdVDimension.MustBeNonnegative := true;
  AddField(FFdVDimension);

  FFdVKnot := TMFDouble.Create(Self, False, 'vKnot', []);
   FdVKnot.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdVKnot);

  FFdVOrder := TSFInt32.Create(Self, False, 'vOrder', 3);
   FdVOrder.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdVOrder);
end;

{$endif read_implementation}
