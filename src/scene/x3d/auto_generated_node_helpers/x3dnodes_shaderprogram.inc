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
    function GetShaderType: TShaderType;
    procedure SetShaderType(const Value: TShaderType);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FFdUrl: TMFString;
    { Internal wrapper for property @code(Url). This wrapper API may change, we advise to access simpler @code(Url) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdUrl: TMFString read FFdUrl;
    {  }
    procedure SetUrl(const Value: array of string); overload;
    {  }
    procedure SetUrl(const Value: TCastleStringList); overload;

    strict private FFdType: TSFStringEnum;
    { Internal wrapper for property @code(ShaderType). This wrapper API may change, we advise to access simpler @code(ShaderType) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdType: TSFStringEnum read FFdType;
    {  }
    property ShaderType: TShaderType read GetShaderType write SetShaderType;

{$endif read_interface}

{$ifdef read_implementation}

{ TShaderProgramNode ----------------------------------------------- }

procedure TShaderProgramNode.SetUrl(const Value: array of string);
begin
  FdUrl.Send(Value);
end;

procedure TShaderProgramNode.SetUrl(const Value: TCastleStringList);
begin
  FdUrl.Send(Value);
end;

function TShaderProgramNode.GetShaderType: TShaderType;
begin
  Result := TShaderType(FdType.EnumValue);
end;

procedure TShaderProgramNode.SetShaderType(const Value: TShaderType);
begin
  FdType.SendEnumValue(Ord(Value));
end;

class function TShaderProgramNode.ClassX3DType: String;
begin
  Result := 'ShaderProgram';
end;

procedure TShaderProgramNode.CreateNode;
begin
  inherited;

  FFdUrl := TMFString.Create(Self, True, 'url', []);
   FdUrl.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdUrl);

  FFdType := TSFStringEnum.Create(Self, False, 'type', @ShaderTypeNameX3D, Ord(High(ShaderTypeNameX3D)) + 1, Ord(stVertex));
   FdType.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdType);

  DefaultContainerField := 'programs';
end;

{$endif read_implementation}
