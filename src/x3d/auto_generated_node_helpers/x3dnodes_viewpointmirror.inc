{ -*- buffer-read-only: t -*-

  Copyright 2015-2018 Michalis Kamburelis.

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
    function GetDistanceFromShape: Single;
    procedure SetDistanceFromShape(const Value: Single);
  public
    procedure CreateNode; override;

    strict private FFdDistanceFromShape: TSFFloat;
    { X3D field "distanceFromShape". } { }
    public property FdDistanceFromShape: TSFFloat read FFdDistanceFromShape;
    { X3D field "distanceFromShape". } { }
    property DistanceFromShape: Single read GetDistanceFromShape write SetDistanceFromShape;

{$endif read_interface}

{$ifdef read_implementation}

{ TViewpointMirrorNode ----------------------------------------------- }

function TViewpointMirrorNode.GetDistanceFromShape: Single;
begin
  Result := FdDistanceFromShape.Value;
end;

procedure TViewpointMirrorNode.SetDistanceFromShape(const Value: Single);
begin
  FdDistanceFromShape.Send(Value);
end;

procedure TViewpointMirrorNode.CreateNode;
begin
  inherited;

  FFdDistanceFromShape := TSFFloat.Create(Self, True, 'distanceFromShape', 0.01);
   FdDistanceFromShape.ChangesAlways := [chVisibleNonGeometry]; // TODO: adjust if necessary
  AddField(FFdDistanceFromShape);

  DefaultContainerField := 'viewpoint';
end;

{$endif read_implementation}
