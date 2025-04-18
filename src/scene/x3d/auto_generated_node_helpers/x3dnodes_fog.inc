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
    function GetColor: TCastleColorRGB;
    procedure SetColor(const Value: TCastleColorRGB);
    function GetFogType: TFogType;
    procedure SetFogType(const Value: TFogType);
    function GetVisibilityRange: Single;
    procedure SetVisibilityRange(const Value: Single);
    function GetVolumetric: Boolean;
    procedure SetVolumetric(const Value: Boolean);
    function GetVolumetricDirection: TVector3;
    procedure SetVolumetricDirection(const Value: TVector3);
    function GetVolumetricVisibilityStart: Single;
    procedure SetVolumetricVisibilityStart(const Value: Single);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FFdColor: TSFColor;
    { Internal wrapper for property @code(Color). This wrapper API may change, we advise to access simpler @code(Color) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdColor: TSFColor read FFdColor;
    {  }
    property Color: TCastleColorRGB read GetColor write SetColor;

    strict private FFdFogType: TSFStringEnum;
    { Internal wrapper for property @code(FogType). This wrapper API may change, we advise to access simpler @code(FogType) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdFogType: TSFStringEnum read FFdFogType;
    {  }
    property FogType: TFogType read GetFogType write SetFogType;

    strict private FFdVisibilityRange: TSFFloat;
    { Internal wrapper for property @code(VisibilityRange). This wrapper API may change, we advise to access simpler @code(VisibilityRange) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdVisibilityRange: TSFFloat read FFdVisibilityRange;
    {  }
    property VisibilityRange: Single read GetVisibilityRange write SetVisibilityRange;

    strict private FFdVolumetric: TSFBool;
    { Internal wrapper for property @code(Volumetric). This wrapper API may change, we advise to access simpler @code(Volumetric) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdVolumetric: TSFBool read FFdVolumetric;
    { Activate volumetric fog, see https://castle-engine.io/x3d_extensions.php }
    property Volumetric: Boolean read GetVolumetric write SetVolumetric;

    strict private FFdVolumetricDirection: TSFVec3f;
    { Internal wrapper for property @code(VolumetricDirection). This wrapper API may change, we advise to access simpler @code(VolumetricDirection) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdVolumetricDirection: TSFVec3f read FFdVolumetricDirection;
    {  }
    property VolumetricDirection: TVector3 read GetVolumetricDirection write SetVolumetricDirection;

    strict private FFdVolumetricVisibilityStart: TSFFloat;
    { Internal wrapper for property @code(VolumetricVisibilityStart). This wrapper API may change, we advise to access simpler @code(VolumetricVisibilityStart) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdVolumetricVisibilityStart: TSFFloat read FFdVolumetricVisibilityStart;
    {  }
    property VolumetricVisibilityStart: Single read GetVolumetricVisibilityStart write SetVolumetricVisibilityStart;

{$endif read_interface}

{$ifdef read_implementation}

{ TFogNode ----------------------------------------------- }

function TFogNode.GetColor: TCastleColorRGB;
begin
  Result := FdColor.Value;
end;

procedure TFogNode.SetColor(const Value: TCastleColorRGB);
begin
  FdColor.Send(Value);
end;

function TFogNode.GetFogType: TFogType;
begin
  Result := TFogType(FdFogType.EnumValue);
end;

procedure TFogNode.SetFogType(const Value: TFogType);
begin
  FdFogType.SendEnumValue(Ord(Value));
end;

function TFogNode.GetVisibilityRange: Single;
begin
  Result := FdVisibilityRange.Value;
end;

procedure TFogNode.SetVisibilityRange(const Value: Single);
begin
  FdVisibilityRange.Send(Value);
end;

function TFogNode.GetVolumetric: Boolean;
begin
  Result := FdVolumetric.Value;
end;

procedure TFogNode.SetVolumetric(const Value: Boolean);
begin
  FdVolumetric.Send(Value);
end;

function TFogNode.GetVolumetricDirection: TVector3;
begin
  Result := FdVolumetricDirection.Value;
end;

procedure TFogNode.SetVolumetricDirection(const Value: TVector3);
begin
  FdVolumetricDirection.Send(Value);
end;

function TFogNode.GetVolumetricVisibilityStart: Single;
begin
  Result := FdVolumetricVisibilityStart.Value;
end;

procedure TFogNode.SetVolumetricVisibilityStart(const Value: Single);
begin
  FdVolumetricVisibilityStart.Send(Value);
end;

class function TFogNode.ClassX3DType: String;
begin
  Result := 'Fog';
end;

procedure TFogNode.CreateNode;
begin
  inherited;

  FFdColor := TSFColor.Create(Self, True, 'color', Vector3(1, 1, 1));
   FdColor.ChangeAlways := chRedisplay;
  AddField(FFdColor);

  FFdFogType := TSFStringEnum.Create(Self, True, 'fogType', @FogTypeNames, Ord(High(FogTypeNames)) + 1, Ord(ftLinear));
   FdFogType.ChangeAlways := chRedisplay;
  AddField(FFdFogType);

  FFdVisibilityRange := TSFFloat.Create(Self, True, 'visibilityRange', 0);
   FdVisibilityRange.ChangeAlways := chRedisplay;
   FdVisibilityRange.MustBeNonnegative := true;
  AddField(FFdVisibilityRange);

  FFdVolumetric := TSFBool.Create(Self, True, 'volumetric', false);
   FdVolumetric.ChangeAlways := chRedisplay;
  AddField(FFdVolumetric);

  FFdVolumetricDirection := TSFVec3f.Create(Self, True, 'volumetricDirection', Vector3(0, -1, 0));
   FdVolumetricDirection.ChangeAlways := chRedisplay;
  AddField(FFdVolumetricDirection);

  FFdVolumetricVisibilityStart := TSFFloat.Create(Self, True, 'volumetricVisibilityStart', 0);
   FdVolumetricVisibilityStart.ChangeAlways := chRedisplay;
  AddField(FFdVolumetricVisibilityStart);
end;

{$endif read_implementation}
