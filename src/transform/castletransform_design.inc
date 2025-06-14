{%MainUnit castletransform.pas}
{
  Copyright 2017-2025 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{$ifdef read_interface}

// type section started in CastleTransform.pas

  EMaximumTransformDesignDepth = class(Exception);

  { Contents are loaded from an indicated castle-transform file
    (by the @link(Url) property).

    The file should be in the format created by Castle Game Engine Editor
    (serialized by CastleComponentSerialize routines) with .castle-transform extension. }
  TCastleTransformDesign = class(TCastleTransform)
  strict private
    class var
      LoadDepth: Cardinal;
    var
      { Intermediate TCastleTransform that is child of Self and contains FDesign.
        This way FDesignContainer is csTransient, not FDesign.
        This allows to detect hitting FDesign instance with Viewport.TransformHit,
        useful if FDesign is a TCastleScene.
        Testcase: Enter to push on https://github.com/castle-engine/conference-delphi-summit-2025 }
      FDesignContainer: TCastleTransform;
      FDesign: TCastleTransform;
      { We don't use Self as Owner of FDesign, to allow freeing everything
        loaded by TransformLoad by simple FreeAndNil(FDesignOwner). }
      FDesignOwner: TComponent;
      FDesignObserver: TFreeNotificationObserver;
      FUrl: String;
      FUrlMonitoring: TUrlMonitoring;
      FRevertUrl: String;
    procedure SetUrl(const Value: String);
    procedure DesignFreeNotification(const Sender: TFreeNotificationObserver);
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    function PropertySections(const PropertyName: String): TPropertySections; override;
    procedure BeforeDestruction; override;

    { Find by name a component in currently loaded design.
      When the name is not found, raises exception (unless Required is @false,
      then it returns @nil). }
    function DesignedComponent(const ComponentName: String;
      const Required: Boolean = true): TComponent;

    function FindRequiredComponent(const ComponentName: String): TComponent; deprecated 'use DesignedComponent(ComponentName)';
    function FindComponent(const ComponentName: String): TComponent; deprecated 'use DesignedComponent(ComponentName, false)';

    { Load again the design from @link(Url).
      This makes sense to be used when underlying file on disk
      changed, and you want to reload it. }
    procedure ReloadUrl;

    { Root of the loaded components, or @nil if nothing was loaded yet. }
    property DesignRoot: TCastleTransform read FDesign;
  published
    { Load file with the transform design.
      The file should be in the format created by Castle Game Engine Editor
      (see CastleComponentSerialize unit) with .castle-transform extension.
      Setting this property automatically unloads the previous design,
      and loads a new one (if the value is <> '').

      After loading a design, you can use @link(DesignedComponent)
      to find the components inside.
      So e.g. this works:

      @longCode(#
      var
        MyDesign: TCastleTransformDesign;
        MyScene: TCastleScene;
      begin
        MyDesign := TCastleTransformDesign.Create(...);
        MyDesign.Url := 'castle-data:/my_transform.castle-transform';
        MyScene := MyDesign.DesignedComponent('MyScene') as TCastleScene;
        MyScene.Translation := Vector3(1, 2, 3);
      end;
      #)
    }
    property Url: String read FUrl write SetUrl;

    { URL used to perform editor "Revert" operation. }
    property RevertUrl: String read FRevertUrl write FRevertUrl;
  end;

{$endif read_interface}

{$ifdef read_implementation}

{ TCastleTransformDesign ----------------------------------------------------- }

constructor TCastleTransformDesign.Create(AOwner: TComponent);
begin
  inherited;
  FDesignObserver := TFreeNotificationObserver.Create(Self);
  FDesignObserver.OnFreeNotification := {$ifdef FPC}@{$endif}DesignFreeNotification;
  FUrlMonitoring.Init({$ifdef FPC}@{$endif} ReloadUrl);

  FDesignContainer := TCastleTransform.Create(Self);
  { This prevents FDesignContainer and FDesign from
    - being serialized (in case TCastleTransformDesign is placed inside
      a hierachy saved using TransformSave. Only URL should be saved.)
    - or even be visible in the hierachy of castle-editor. }
  FDesignContainer.SetTransient;
  Add(FDesignContainer);
end;

destructor TCastleTransformDesign.Destroy;
begin
  inherited;
end;

procedure TCastleTransformDesign.BeforeDestruction;
begin
  FUrlMonitoring.Finish(FUrl);
  inherited;
end;

procedure TCastleTransformDesign.DesignFreeNotification(const Sender: TFreeNotificationObserver);
begin
  FDesign := nil;
end;

procedure TCastleTransformDesign.SetUrl(const Value: String);
const
  MaxLoadDepth = 64; // 128 is too much for macOS/x86_64; although for Linux/x86_64 even 1024 was OK
begin
  if FUrl <> Value then
  begin
    if FDesign <> nil then
    begin
      FreeAndNil(FDesignOwner);
      Assert(FDesign = nil); // DesignFreeNotification should set FDesign to nil
    end;

    { TODO: Disable auto-reloading of designs for now, fearing it will cause
      more problems than gains: reloading the design invalidates all
      the references to components inside. }
    //FUrlMonitoring.ChangeUrl(FUrl, Value);
    FUrl := Value;

    if Value <> '' then
    begin
      try
        { Check LoadDepth = MaxLoadDepth to prevent infinite loop when referencing your own design. }
        if LoadDepth = MaxLoadDepth then
          raise EMaximumTransformDesignDepth.CreateFmt('Exceeded maximum depth (%d) when loading TCastleTransformDesign within TCastleTransformDesign. It is possible that TCastleTransformDesign references its own design', [
            MaxLoadDepth
          ]);
        Inc(LoadDepth);
        try
          FDesignOwner := TComponent.Create(Self);
          FDesign := TransformLoad(Value, FDesignOwner);
        finally Dec(LoadDepth) end;
      except
        { If loading design file failed, and we're inside CGE editor,
          merely report a warning. This allows deserializing in CGE editor
          designs with broken URLs. }
        on E: Exception do
        begin
          if CastleDesignMode then
          begin
            WritelnWarning('TCastleTransformDesign', 'Failed to load design "%s": %s', [UriDisplay(Url), ExceptMessage(E)]);
            Exit;
          end else
            raise;
        end;
      end;

      FDesignObserver.Observed := FDesign;
      FDesignContainer.Add(FDesign);
    end;
  end;
end;

function TCastleTransformDesign.PropertySections(
  const PropertyName: String): TPropertySections;
begin
  if PropertyName = 'Url' then
    Result := [psBasic]
  else
    Result := inherited PropertySections(PropertyName);
end;

function TCastleTransformDesign.DesignedComponent(const ComponentName: String;
  const Required: Boolean): TComponent;
begin
  if FDesignOwner <> nil then
    Result := FDesignOwner.FindComponent(ComponentName)
  else
    Result := nil;

  if Required and (Result = nil) then
    raise EComponentNotFound.CreateFmt('Cannot find component named "%s" in design "%s"', [ComponentName, Name]);
end;

function TCastleTransformDesign.FindRequiredComponent(const ComponentName: String): TComponent;
begin
  Result := DesignedComponent(ComponentName, true);
end;

function TCastleTransformDesign.FindComponent(const ComponentName: String): TComponent;
begin
  Result := DesignedComponent(ComponentName, false);
end;

procedure TCastleTransformDesign.ReloadUrl;
var
  SavedUrl: String;
begin
  SavedUrl := Url;
  Url := '';
  Url := SavedUrl;
end;

{$endif read_implementation}
