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
    function GetFieldOfView: Single;
    procedure SetFieldOfView(const Value: Single);
    function GetHeadlight: Boolean;
    procedure SetHeadlight(const Value: Boolean);
    function GetGeoOrigin: TGeoOriginNode;
    procedure SetGeoOrigin(const Value: TGeoOriginNode);
    function GetSpeedFactor: Single;
    procedure SetSpeedFactor(const Value: Single);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FEventSet_orientation: TSFRotationEvent;
    {  }
    public property EventSet_orientation: TSFRotationEvent read FEventSet_orientation;

    strict private FEventSet_position: TSFVec3dEvent;
    {  }
    public property EventSet_position: TSFVec3dEvent read FEventSet_position;

    strict private FFdFieldOfView: TSFFloat;
    { Internal wrapper for property @code(FieldOfView). This wrapper API may change, we advise to access simpler @code(FieldOfView) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdFieldOfView: TSFFloat read FFdFieldOfView;
    {  }
    property FieldOfView: Single read GetFieldOfView write SetFieldOfView;

    strict private FFdHeadlight: TSFBool;
    { Internal wrapper for property @code(Headlight). This wrapper API may change, we advise to access simpler @code(Headlight) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdHeadlight: TSFBool read FFdHeadlight;
    {  }
    property Headlight: Boolean read GetHeadlight write SetHeadlight;

    strict private FFdNavType: TMFString;
    { Internal wrapper for property @code(NavType). This wrapper API may change, we advise to access simpler @code(NavType) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdNavType: TMFString read FFdNavType;
    {  }
    procedure SetNavType(const Value: array of string); overload;
    {  }
    procedure SetNavType(const Value: TCastleStringList); overload;

    strict private FFdGeoOrigin: TSFNode;
    { Internal wrapper for property @code(GeoOrigin). This wrapper API may change, we advise to access simpler @code(GeoOrigin) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdGeoOrigin: TSFNode read FFdGeoOrigin;
    {  }
    property GeoOrigin: TGeoOriginNode read GetGeoOrigin write SetGeoOrigin;

    strict private FFdGeoSystem: TMFString;
    { Internal wrapper for property @code(GeoSystem). This wrapper API may change, we advise to access simpler @code(GeoSystem) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdGeoSystem: TMFString read FFdGeoSystem;
    {  }
    procedure SetGeoSystem(const Value: array of string); overload;
    {  }
    procedure SetGeoSystem(const Value: TCastleStringList); overload;

    strict private FFdPosition: TSFVec3d;
    { Internal wrapper for property @code(Position). This wrapper API may change, we advise to access simpler @code(Position) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdPosition: TSFVec3d read FFdPosition;

    strict private FFdSpeedFactor: TSFFloat;
    { Internal wrapper for property @code(SpeedFactor). This wrapper API may change, we advise to access simpler @code(SpeedFactor) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdSpeedFactor: TSFFloat read FFdSpeedFactor;
    {  }
    property SpeedFactor: Single read GetSpeedFactor write SetSpeedFactor;

{$endif read_interface}

{$ifdef read_implementation}

{ TGeoViewpointNode ----------------------------------------------- }

function TGeoViewpointNode.GetFieldOfView: Single;
begin
  Result := FdFieldOfView.Value;
end;

procedure TGeoViewpointNode.SetFieldOfView(const Value: Single);
begin
  FdFieldOfView.Send(Value);
end;

function TGeoViewpointNode.GetHeadlight: Boolean;
begin
  Result := FdHeadlight.Value;
end;

procedure TGeoViewpointNode.SetHeadlight(const Value: Boolean);
begin
  FdHeadlight.Send(Value);
end;

procedure TGeoViewpointNode.SetNavType(const Value: array of string);
begin
  FdNavType.Send(Value);
end;

procedure TGeoViewpointNode.SetNavType(const Value: TCastleStringList);
begin
  FdNavType.Send(Value);
end;

function TGeoViewpointNode.GetGeoOrigin: TGeoOriginNode;
begin
  if FdGeoOrigin.Value is TGeoOriginNode then
    Result := TGeoOriginNode(FdGeoOrigin.Value)
  else
    Result := nil;
end;

procedure TGeoViewpointNode.SetGeoOrigin(const Value: TGeoOriginNode);
begin
  FdGeoOrigin.Send(Value);
end;

procedure TGeoViewpointNode.SetGeoSystem(const Value: array of string);
begin
  FdGeoSystem.Send(Value);
end;

procedure TGeoViewpointNode.SetGeoSystem(const Value: TCastleStringList);
begin
  FdGeoSystem.Send(Value);
end;

function TGeoViewpointNode.GetSpeedFactor: Single;
begin
  Result := FdSpeedFactor.Value;
end;

procedure TGeoViewpointNode.SetSpeedFactor(const Value: Single);
begin
  FdSpeedFactor.Send(Value);
end;

class function TGeoViewpointNode.ClassX3DType: String;
begin
  Result := 'GeoViewpoint';
end;

procedure TGeoViewpointNode.CreateNode;
begin
  inherited;

  FEventSet_orientation := TSFRotationEvent.Create(Self, 'set_orientation', true);
  AddEvent(FEventSet_orientation);

  FEventSet_position := TSFVec3dEvent.Create(Self, 'set_position', true);
  AddEvent(FEventSet_position);

  FFdFieldOfView := TSFFloat.Create(Self, True, 'fieldOfView', Pi/4);
   FdFieldOfView.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdFieldOfView);

  FFdHeadlight := TSFBool.Create(Self, True, 'headlight', true);
   FdHeadlight.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdHeadlight);

  FFdNavType := TMFString.Create(Self, True, 'navType', ['EXAMINE','ANY']);
   FdNavType.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdNavType);

  FFdGeoOrigin := TSFNode.Create(Self, False, 'geoOrigin', [TGeoOriginNode]);
   FdGeoOrigin.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdGeoOrigin);

  FFdGeoSystem := TMFString.Create(Self, False, 'geoSystem', ['GD','WE']);
   FdGeoSystem.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdGeoSystem);

  FFdPosition := TSFVec3d.Create(Self, False, 'position', Vector3Double(0, 0, 100000));
   FdPosition.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdPosition);

  FFdSpeedFactor := TSFFloat.Create(Self, False, 'speedFactor', 1.0);
   FdSpeedFactor.ChangeAlways := chVisibleNonGeometry;
   FdSpeedFactor.MustBeNonnegative := true;
  AddField(FFdSpeedFactor);
end;

{$endif read_implementation}
