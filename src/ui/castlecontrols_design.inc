{%MainUnit castlecontrols.pas}
{
  Copyright 2018-2024 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{$ifdef read_interface}

  EMaximumDesignDepth = class(Exception);

  { Contents of this user-interface control are loaded from an indicated file
    (by the @link(Url) property).

    The file should be in the format created by Castle Game Engine Editor
    (serialized by CastleComponentSerialize routines) with .castle-user-interface extension. }
  TCastleDesign = class(TCastleUserInterface)
  strict private
    class var
      { Keeps track of TCastleDesign causing loading of another TCastleDesign,
        avoids infinite recursion.

        In the past we tried with MaxDepth, but this
        - Puts unnecessary limit on how deep you can nest designs,
          even when not causing recursion.
        - It's hard to choose safe and reasonable value for MaxDepth.
          In some cases 1024 was enough, in some 32, in some even 32 caused
          too much trouble.
        Using a stack with URLs allows to break at 1st sight of recursion,
        but also allow any depth when it's not recursive. }
      LoadStack: TCastleStringList;
    var
      FDesign: TCastleUserInterface;
      { We don't use Self as Owner of FDesign, to allow freeing everything
        loaded by UserInterfaceLoad by simple FreeAndNil(FDesignOwner). }
      FDesignOwner: TComponent;
      FDesignObserver: TFreeNotificationObserver;
      FUrl: String;
      FUrlMonitoring: TUrlMonitoring;
      FRevertUrl: String;
      FStretchWhenNotLoaded: Boolean;
    procedure SetUrl(const Value: String);
    function GetStretch: Boolean;
    procedure SetStretch(const Value: Boolean);
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

    { Loaded design, only for internal usage. @exclude }
    property InternalDesign: TCastleUserInterface read FDesign;

    { Load again the design from @link(Url).
      This makes sense to be used when underlying file on disk
      changed, and you want to reload it. }
    procedure ReloadUrl;

    { Root of the loaded components, or @nil if nothing was loaded yet. }
    property DesignRoot: TCastleUserInterface read FDesign;
  published
    { Load file with the user interface design.
      The file should be in the format created by Castle Game Engine Editor
      (see CastleComponentSerialize unit) with .castle-user-interface extension.
      Setting this property automatically unloads the previous design,
      and loads a new one (if the value is <> '').

      After loading a design, you can use @link(DesignedComponent)
      to find the components inside.
      So e.g. this works:

      @longCode(#
      var
        MyDesign: TCastleDesign;
        MyButton: TCastleButton;
      begin
        MyDesign := TCastleDesign.Create(...);
        MyDesign.Url := 'castle-data:/my_user_interface.castle-user-interface';
        MyButton := MyDesign.DesignedComponent('MyButton') as TCastleButton;
        MyButton.OnClick := ...;
      end;
      #)
    }
    property Url: String read FUrl write SetUrl;

    { Whether the loaded component (from @link(Url)) fills the rectangle
      of this TCastleDesign instance.

      By default this is @true, which makes sense if the loaded component
      can be freely stretched. In this case, the loaded component will have
      @link(FullSize) set to @true, so it fills this TCastleDesign instance.
      And you can control the size of everything by controlling the size
      of this TCastleDesign instance.

      You can set this to @false if the loaded component should not
      be arbitrarily stretched. E.g. maybe it is a TCastleImageControl
      with @link(TCastleImageControl.Stretch) = @true.
      Maybe it is TCastleButton with @link(TCastleButton.AutoSize) = @true.
      While every component can be forced to be stretched (by setting FullSize
      to @true), it may look bad in some circumstances.
      In such case, you should set this property to @false.
      You can also set @link(AutoSizeToChildren) to @true, to adjust the size of this
      TCastleDesign instance to the child.
    }
    property Stretch: Boolean read GetStretch write SetStretch default true;

    { URL used to perform editor "Revert" operation. }
    property RevertUrl: String read FRevertUrl write FRevertUrl;
  end;

{$endif read_interface}

{$ifdef read_implementation}

{ TCastleDesign -------------------------------------------------------------- }

constructor TCastleDesign.Create(AOwner: TComponent);
begin
  inherited;
  FDesignObserver := TFreeNotificationObserver.Create(Self);
  FDesignObserver.OnFreeNotification := {$ifdef FPC}@{$endif}DesignFreeNotification;
  FStretchWhenNotLoaded := true;
  FUrlMonitoring.Init({$ifdef FPC}@{$endif} ReloadUrl);
end;

destructor TCastleDesign.Destroy;
begin
  inherited;
end;

procedure TCastleDesign.BeforeDestruction;
begin
  FUrlMonitoring.Finish(FUrl);
  inherited;
end;

procedure TCastleDesign.DesignFreeNotification(const Sender: TFreeNotificationObserver);
begin
  FDesign := nil;
end;

procedure TCastleDesign.SetUrl(const Value: String);

  { Make a warning if LoadStack check fails.
    Otherwise add us to LoadStack. }
  procedure CheckAndBeingLoadStack;
  begin
    if LoadStack = nil then
      LoadStack := TCastleStringList.Create;

    if LoadStack.IndexOf(Url) <> -1 then
      raise EMaximumDesignDepth.CreateFmt('TCastleDesign %s with URL "%s" loads recursively the same design file. Breaking, to prevent infinite recursion', [
        Name, Url
      ]);
    LoadStack.Add(Url);
  end;

  { Remove us from LoadStack. }
  procedure EndLoadStack;
  var
    I: Integer;
  begin
    I := LoadStack.IndexOf(Url);
    Assert(LoadStack.Count <> 0);
    Assert(I = LoadStack.Count - 1);
    LoadStack.Delete(I);
    if LoadStack.Count = 0 then
      FreeAndNil(LoadStack);
  end;

begin
  if FUrl <> Value then
  begin
    if FDesign <> nil then
    begin
      FStretchWhenNotLoaded := FDesign.FullSize;
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
        CheckAndBeingLoadStack;
        try
          FDesignOwner := TComponent.Create(Self);
          FDesign := UserInterfaceLoad(Value, FDesignOwner);
        finally EndLoadStack end;
      except
        { If loading design file failed, and we're inside CGE editor,
          merely report a warning. This allows deserializing in CGE editor
          designs with broken URLs. }
        on E: Exception do
        begin
          if CastleDesignMode then
          begin
            WritelnWarning('TCastleDesign', 'Failed to load design "%s": %s', [UriDisplay(Url), ExceptMessage(E)]);
            Exit;
          end else
            raise;
        end;
      end;

      { This prevents this child from
        - being serialized (in case TCastleDesign is placed inside
          a hierachy saved using UserInterfaceSave. Only URL should be saved.)
        - or even be visible in the hierachy of castle-editor.
      }
      FDesign.SetTransient;
      FDesign.FullSize := FStretchWhenNotLoaded;
      FDesignObserver.Observed := FDesign;
      InsertFront(FDesign);
    end;
  end;
end;

function TCastleDesign.GetStretch: Boolean;
begin
  { Keep this property synchronized with FDesign.FullSize, when FDesign exists.
    This way when user changes FDesign.FullSize directly,
    then the TCastleDesign.Stretch property also changes automatically. }
  if FDesign <> nil then
    Result := FDesign.FullSize
  else
    Result := FStretchWhenNotLoaded;
end;

procedure TCastleDesign.SetStretch(const Value: Boolean);
begin
  if FDesign <> nil then
    FDesign.FullSize := Value
  else
    FStretchWhenNotLoaded := Value;
end;

function TCastleDesign.PropertySections(
  const PropertyName: String): TPropertySections;
begin
  if (PropertyName = 'Url') or
     (PropertyName = 'Stretch') then
    Result := [psBasic]
  else
    Result := inherited PropertySections(PropertyName);
end;

function TCastleDesign.DesignedComponent(const ComponentName: String;
  const Required: Boolean): TComponent;
begin
  if FDesignOwner <> nil then
    Result := FDesignOwner.FindComponent(ComponentName)
  else
    Result := nil;

  if Required and (Result = nil) then
    raise EComponentNotFound.CreateFmt('Cannot find component named "%s" in design "%s"', [ComponentName, Name]);
end;

function TCastleDesign.FindRequiredComponent(const ComponentName: String): TComponent;
begin
  Result := DesignedComponent(ComponentName, true);
end;

function TCastleDesign.FindComponent(const ComponentName: String): TComponent;
begin
  Result := DesignedComponent(ComponentName, false);
end;

procedure TCastleDesign.ReloadUrl;
var
  SavedUrl: String;
begin
  SavedUrl := Url;
  Url := '';
  Url := SavedUrl;
end;

{$endif read_implementation}
