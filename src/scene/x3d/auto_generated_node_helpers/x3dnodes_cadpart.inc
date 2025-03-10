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
    function GetCenter: TVector3;
    procedure SetCenter(const Value: TVector3);
    function GetNameField: String;
    procedure SetNameField(const Value: String);
    function GetRotation: TVector4;
    procedure SetRotation(const Value: TVector4);
    function GetScale: TVector3;
    procedure SetScale(const Value: TVector3);
    function GetScaleOrientation: TVector4;
    procedure SetScaleOrientation(const Value: TVector4);
    function GetTranslation: TVector3;
    procedure SetTranslation(const Value: TVector3);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FFdCenter: TSFVec3f;
    { Internal wrapper for property @code(Center). This wrapper API may change, we advise to access simpler @code(Center) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdCenter: TSFVec3f read FFdCenter;
    {  }
    property Center: TVector3 read GetCenter write SetCenter;

    strict private FFdName: TSFString;
    { Internal wrapper for property @code(NameField). This wrapper API may change, we advise to access simpler @code(NameField) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdName: TSFString read FFdName;
    {  }
    property NameField: String read GetNameField write SetNameField;

    strict private FFdRotation: TSFRotation;
    { Internal wrapper for property @code(Rotation). This wrapper API may change, we advise to access simpler @code(Rotation) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdRotation: TSFRotation read FFdRotation;
    {  }
    property Rotation: TVector4 read GetRotation write SetRotation;

    strict private FFdScale: TSFVec3f;
    { Internal wrapper for property @code(Scale). This wrapper API may change, we advise to access simpler @code(Scale) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdScale: TSFVec3f read FFdScale;
    {  }
    property Scale: TVector3 read GetScale write SetScale;

    strict private FFdScaleOrientation: TSFRotation;
    { Internal wrapper for property @code(ScaleOrientation). This wrapper API may change, we advise to access simpler @code(ScaleOrientation) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdScaleOrientation: TSFRotation read FFdScaleOrientation;
    {  }
    property ScaleOrientation: TVector4 read GetScaleOrientation write SetScaleOrientation;

    strict private FFdTranslation: TSFVec3f;
    { Internal wrapper for property @code(Translation). This wrapper API may change, we advise to access simpler @code(Translation) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdTranslation: TSFVec3f read FFdTranslation;
    {  }
    property Translation: TVector3 read GetTranslation write SetTranslation;

{$endif read_interface}

{$ifdef read_implementation}

{ TCADPartNode ----------------------------------------------- }

function TCADPartNode.GetCenter: TVector3;
begin
  Result := FdCenter.Value;
end;

procedure TCADPartNode.SetCenter(const Value: TVector3);
begin
  FdCenter.Send(Value);
end;

function TCADPartNode.GetNameField: String;
begin
  Result := FdName.Value;
end;

procedure TCADPartNode.SetNameField(const Value: String);
begin
  FdName.Send(Value);
end;

function TCADPartNode.GetRotation: TVector4;
begin
  Result := FdRotation.Value;
end;

procedure TCADPartNode.SetRotation(const Value: TVector4);
begin
  FdRotation.Send(Value);
end;

function TCADPartNode.GetScale: TVector3;
begin
  Result := FdScale.Value;
end;

procedure TCADPartNode.SetScale(const Value: TVector3);
begin
  FdScale.Send(Value);
end;

function TCADPartNode.GetScaleOrientation: TVector4;
begin
  Result := FdScaleOrientation.Value;
end;

procedure TCADPartNode.SetScaleOrientation(const Value: TVector4);
begin
  FdScaleOrientation.Send(Value);
end;

function TCADPartNode.GetTranslation: TVector3;
begin
  Result := FdTranslation.Value;
end;

procedure TCADPartNode.SetTranslation(const Value: TVector3);
begin
  FdTranslation.Send(Value);
end;

class function TCADPartNode.ClassX3DType: String;
begin
  Result := 'CADPart';
end;

procedure TCADPartNode.CreateNode;
begin
  inherited;

  FFdCenter := TSFVec3f.Create(Self, True, 'center', Vector3(0, 0, 0));
   FdCenter.ChangeAlways := chTransform;
  AddField(FFdCenter);

  FFdName := TSFString.Create(Self, True, 'name', '');
   FdName.ChangeAlways := chNone;
  AddField(FFdName);

  FFdRotation := TSFRotation.Create(Self, True, 'rotation', Vector3(0, 0, 1), 0);
   FdRotation.ChangeAlways := chTransform;
  AddField(FFdRotation);

  FFdScale := TSFVec3f.Create(Self, True, 'scale', Vector3(1, 1, 1));
   FdScale.ChangeAlways := chTransform;
  AddField(FFdScale);

  FFdScaleOrientation := TSFRotation.Create(Self, True, 'scaleOrientation', Vector3(0, 0, 1), 0);
   FdScaleOrientation.ChangeAlways := chTransform;
  AddField(FFdScaleOrientation);

  FFdTranslation := TSFVec3f.Create(Self, True, 'translation', Vector3(0, 0, 0));
   FdTranslation.ChangeAlways := chTransform;
  AddField(FFdTranslation);
end;

{$endif read_implementation}
