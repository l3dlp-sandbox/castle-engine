{%MainUnit castlecontrols.pas}
{
  Copyright 2017-2018 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{$ifdef read_interface}

  { Edit box to input 1 line of text.

    The edit control @link(Width TCastleUserInterface.Width)
    is never auto-calculated, since there's no "natural width" for the edit box.
    We use the property @link(Width TCastleUserInterface.Width) to determine
    width. The @link(Width TCastleUserInterface.Width) must include also
    space for the border frame (if @link(Frame) is @true),
    and padding (see @link(Padding) + @link(PaddingHorizontal)).
    So, we are similar to CSS box model called box-sizing, width includes also
    padding and border.

    In contrast, our height is by default
    (see @link(AutoSizeHeight)) auto-calculated,
    based on frame, padding and font size.
  }
  TCastleEdit = class(TCastleUserInterfaceFont)
  strict private
    FText: string;
    FPaddingHorizontal, FPaddingVertical, FPadding: Single;
    FBackgroundColor, FFocusedColor, FUnfocusedColor: TCastleColor;
    FPlaceholderColor: TCastleColor;
    FFrame: boolean;
    LifeTime: TFloatTime;
    FMaxLength: Cardinal;
    FAllowedChars: TSetOfChars;
    FCaptureAllInput: boolean;
    FAutoOnScreenKeyboard: Boolean;
    Scissor: TScissor;
    FAutoSizeHeight: Boolean;
    FPlaceholder: String;
    FOnChange: TNotifyEvent;
    FPasswordChar: Char;
    procedure SetText(const Value: string);
    procedure SetFocusedColor(const Value: TCastleColor);
    procedure SetUnfocusedColor(const Value: TCastleColor);
    procedure SetPlaceholderColor(const Value: TCastleColor);
    procedure SetBackgroundColor(const Value: TCastleColor);
    procedure SetPadding(const Value: Single);
    procedure SetPaddingHorizontal(const Value: Single);
    procedure SetPaddingVertical(const Value: Single);
    procedure SetFrame(const Value: boolean);
    procedure SetAutoSizeHeight(const Value: Boolean);
    procedure SetPlaceholder(const Value: String);
    procedure SetPasswordChar(const Value: Char);
  protected
    procedure PreferredSize(var PreferredWidth, PreferredHeight: Single); override;
    procedure DoChange; virtual;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    procedure Render; override;
    function Press(const Event: TInputPressRelease): boolean; override;
    procedure Update(const SecondsPassed: Single;
      var HandleInput: boolean); override;
    function CapturesEventsAtPosition(const Position: TVector2): boolean; override;
    procedure SetFocused(const Value: boolean); override;
    procedure EditorAllowResize(
      out ResizeWidth, ResizeHeight: Boolean; out Reason: String); override;

    { Focused text color. By default it's black. }
    property FocusedColor: TCastleColor read FFocusedColor write SetFocusedColor;

    { Unfocused text color. By default it's dark gray. }
    property UnfocusedColor: TCastleColor read FUnfocusedColor write SetUnfocusedColor;

    { Color for @link(Placeholder). By default it's light gray. }
    property PlaceholderColor: TCastleColor read FPlaceholderColor write SetPlaceholderColor;

    { Background color. Fills the area under @link(Text) and the padding.
      By default it's white.
      Note that it can be transparent, in which case it will show the
      tiEdit image underneath (if @link(Frame) is @true),
      or UI control underneath (if @link(Frame) is @false). }
    property BackgroundColor: TCastleColor read FBackgroundColor write SetBackgroundColor;

    { Allowed characters that user can input.
      Note that this @italic(only restricts the user input),
      it does not prevent from programmatically setting @link(Text)
      to include disallowed characters. }
    property AllowedChars: TSetOfChars read FAllowedChars write FAllowedChars;

    { Max length of user input.
      Value of 0 (default) means "no limit".
      Note that this @italic(only restricts the user input),
      it does not prevent from programmatically setting @link(Text)
      to be longer. }
    property MaxLength: Cardinal read FMaxLength write FMaxLength;
  published
    const
      DefaultPadding = 4;
      DefaultAutoOnScreenKeyboard = false;

    { Currently input text. }
    property Text: string read FText write SetText;

    { Inside the box rectangle, padding between the borders (or frame, if @link(Frame)) and text.
      Total horizontal padding is the sum @code(PaddingHorizontal + Padding),
      total vertical padding is the sum @code(PaddingVertical + Padding).
      @groupBegin }
    property PaddingHorizontal: Single
      read FPaddingHorizontal write SetPaddingHorizontal default 0;
    property PaddingVertical: Single
      read FPaddingVertical write SetPaddingVertical default 0;
    property Padding: Single
      read FPadding write SetPadding default DefaultPadding;
    { @groupEnd }

    { Draw frame around the box. Frame uses theme image tiEdit,
      see TCastleTheme.Images if you want to customize it. }
    property Frame: boolean read FFrame write SetFrame default true;

    { Should we capture input regardless of the currently focused control.
      This is in practice only sensible if this is the only edit box currently
      visible on the screen. }
    property CaptureAllInput: boolean read FCaptureAllInput write FCaptureAllInput;

    { When true, it shows the software keyboard on Android. }
    property AutoOnScreenKeyboard: Boolean
      read FAutoOnScreenKeyboard write FAutoOnScreenKeyboard
      default DefaultAutoOnScreenKeyboard;

    property AutoSizeHeight: Boolean
      read FAutoSizeHeight write SetAutoSizeHeight default true;

    { Displayed when @link(Text) is empty. }
    property Placeholder: String
      read FPlaceholder write SetPlaceholder;

    { Event sent when @link(Text) value was changed by a user.
      Note that this is @italic(not) called when you change @link(Text)
      property programmatically. }
    property OnChange: TNotifyEvent read FOnChange write FOnChange;

    { Show given character (usually "*") instead of actual input characters.
      Useful for password input fields. }
    property PasswordChar: Char
      read FPasswordChar write SetPasswordChar default #0;

  {$define read_interface_class}
  {$I auto_generated_persistent_vectors/tcastleedit_persistent_vectors.inc}
  {$undef read_interface_class}
  end;

{$endif read_interface}

{$ifdef read_implementation}

const
  CaretChar = '|';

{ TCastleEdit --------------------------------------------------------------- }

constructor TCastleEdit.Create(AOwner: TComponent);
begin
  inherited;
  FFocusedColor := Black;
  FUnfocusedColor := Vector4(0.25, 0.25, 0.25, 1);
  FPlaceholderColor := Vector4(0.75, 0.75, 0.75, 1);
  FBackgroundColor := White;
  FPadding := DefaultPadding;
  FFrame := true;
  FAllowedChars := AllChars;
  FMaxLength := 0;
  FAutoOnScreenKeyboard := DefaultAutoOnScreenKeyboard;
  Scissor := TScissor.Create;
  FAutoSizeHeight := true;

  {$define read_implementation_constructor}
  {$I auto_generated_persistent_vectors/tcastleedit_persistent_vectors.inc}
  {$undef read_implementation_constructor}
end;

destructor TCastleEdit.Destroy;
begin
  FreeAndNil(Scissor);

  {$define read_implementation_destructor}
  {$I auto_generated_persistent_vectors/tcastleedit_persistent_vectors.inc}
  {$undef read_implementation_destructor}
  inherited;
end;

procedure TCastleEdit.SetText(const Value: string);
begin
  if FText <> Value then
  begin
    FText := Value;
    VisibleChange([chRender]);
  end;
end;

procedure TCastleEdit.SetFocusedColor(const Value: TCastleColor);
begin
  if not TCastleColor.PerfectlyEquals(FFocusedColor, Value) then
  begin
    FFocusedColor := Value;
    VisibleChange([chRender]);
  end;
end;

procedure TCastleEdit.SetUnfocusedColor(const Value: TCastleColor);
begin
  if not TCastleColor.PerfectlyEquals(FUnfocusedColor, Value) then
  begin
    FUnfocusedColor := Value;
    VisibleChange([chRender]);
  end;
end;

procedure TCastleEdit.SetPlaceholderColor(const Value: TCastleColor);
begin
  if not TCastleColor.PerfectlyEquals(FPlaceholderColor, Value) then
  begin
    FPlaceholderColor := Value;
    VisibleChange([chRender]);
  end;
end;

procedure TCastleEdit.SetBackgroundColor(const Value: TCastleColor);
begin
  if not TCastleColor.PerfectlyEquals(FBackgroundColor, Value) then
  begin
    FBackgroundColor := Value;
    VisibleChange([chRender]);
  end;
end;

procedure TCastleEdit.SetPadding(const Value: Single);
begin
  if FPadding <> Value then
  begin
    FPadding := Value;
    VisibleChange([chRectangle]);
  end;
end;

procedure TCastleEdit.SetPaddingHorizontal(const Value: Single);
begin
  if FPaddingHorizontal <> Value then
  begin
    FPaddingHorizontal := Value;
    VisibleChange([chRectangle]);
  end;
end;

procedure TCastleEdit.SetPaddingVertical(const Value: Single);
begin
  if FPaddingVertical <> Value then
  begin
    FPaddingVertical := Value;
    VisibleChange([chRectangle]);
  end;
end;

procedure TCastleEdit.SetFrame(const Value: boolean);
begin
  if FFrame <> Value then
  begin
    FFrame := Value;
    VisibleChange([chRectangle]);
  end;
end;

procedure TCastleEdit.SetAutoSizeHeight(const Value: Boolean);
begin
  if FAutoSizeHeight <> Value then
  begin
    FAutoSizeHeight := Value;
    VisibleChange([chRectangle]);
  end;
end;

procedure TCastleEdit.PreferredSize(var PreferredWidth, PreferredHeight: Single);
var
  PaddingVerticalScaled, HeightScaled: Single;
  US: Single;
  FrameCorners: TVector4Integer;
begin
  inherited;

  if AutoSizeHeight then
  begin
    US := UIScale;
    PaddingVerticalScaled := US * (PaddingVertical + Padding);

    HeightScaled := Font.RowHeight + 2 * PaddingVerticalScaled;
    if Frame then
    begin
      FrameCorners := Theme.Corners[tiEdit];
      HeightScaled := HeightScaled +
        US * FrameCorners[0] +
        US * FrameCorners[2];
    end;

    PreferredHeight := HeightScaled;
  end;
end;

procedure TCastleEdit.DoChange;
begin
  if Assigned(OnChange) then
    OnChange(Self);
end;

procedure TCastleEdit.EditorAllowResize(
  out ResizeWidth, ResizeHeight: Boolean; out Reason: String);
begin
  inherited;
  if AutoSizeHeight then
  begin
    ResizeHeight := false;
    Reason := SAppendPart(Reason, NL, 'Turn off "TCastleEdit.AutoSizeHeight" to change height.');
  end;
end;

procedure TCastleEdit.Render;
const
  CaretSpeed = 1; //< how many blinks per second
var
  SR, RectInside: TFloatRectangle;
  PaddingHorizontalScaled, PaddingVerticalScaled: Single;
  US, TextLeft, TextBottom: Single;
  FrameCorners: TVector4Integer;
  CaretVisible: boolean;
  S: string;
  Color: TCastleColor;
  CaretShift: Integer;
begin
  inherited;

  SR := RenderRect;
  US := UIScale;
  RectInside := SR;

  if Frame then
  begin
    { make smaller RectInside, by frame size }
    FrameCorners := Theme.Corners[tiEdit];
    RectInside := RectInside.
      RemoveTop(FrameCorners[0]).
      RemoveRight(FrameCorners[1]).
      RemoveBottom(FrameCorners[2]).
      RemoveLeft(FrameCorners[3]);

    { draw frame }
    Theme.Draw(SR, tiEdit, US);
  end;

  { fill padding and area under Text with BackgroundColor }
  DrawRectangle(RectInside, BackgroundColor);

  { make smaller RectInside, by padding }
  PaddingHorizontalScaled := US * (PaddingHorizontal + Padding);
  PaddingVerticalScaled := US * (PaddingVertical + Padding);
  RectInside := RectInside.Grow(-PaddingHorizontalScaled, -PaddingVerticalScaled);

  // calculate S, Color, CaretShift
  CaretShift := 0;
  if Text <> '' then
  begin
    S := Text;

    { in case PasswordChar is non-zero, replace S with repeated PasswordChar }
    if PasswordChar <> #0 then
      S := StringOfChar(PasswordChar, Length(S));

    if Focused then
    begin
      { Note that S here may be already processed by PasswordChar. }
      CaretShift := Font.TextWidth(S);
      Color := FocusedColor
    end else
    begin
      Color := UnfocusedColor;
      { CaretShift doesn't matter in this case,
        so we don't have to calculate TextWidth, saving a small time. }
    end;
  end else
  begin
    S := Placeholder;
    Color := PlaceholderColor;
  end;

  Scissor.Rect := RectInside.Round;
  Scissor.Enabled := true;

  TextLeft := RectInside.Left;
  // center in RectInside, important when AutoSize = false
  TextBottom := RectInside.Bottom + (RectInside.Height - Font.RowHeight) / 2;
  Font.Print(TextLeft, TextBottom, Color, S);

  { Note that we have to display Caret even when Text = '' and S = Placeholder. }
  if Focused then
  begin
    CaretVisible := FloatModulo(LifeTime * CaretSpeed, 1.0) < 0.5;
    if CaretVisible then
      Font.Print(TextLeft + CaretShift, TextBottom, FocusedColor, CaretChar);
  end;

  Scissor.Enabled := false;
end;

procedure TCastleEdit.Update(const SecondsPassed: Single;
  var HandleInput: boolean);
begin
  inherited;
  LifeTime := LifeTime + SecondsPassed;
  { do not allow other controls to handle key presses
    (and e.g. move camera when "A" is pressed down), TCastleEdit handles all keys }
  if ExclusiveEvents then
    HandleInput := false;
end;

function TCastleEdit.Press(const Event: TInputPressRelease): boolean;
begin
  Result := inherited;
  if Result or (not GetExists) then Exit;

  if Event.IsKey(CharBackSpace) or Event.IsKey(K_BackSpace) then
  begin
    if Text <> '' then
    begin
      Text := Copy(Text, 1, Length(Text) - 1);
      DoChange;
    end;
    Result := ExclusiveEvents;
  end else
  if Event.IsKey(CtrlC) then
  begin
    if Text <> '' then
      Clipboard.AsText := Text;
    Result := ExclusiveEvents;
  end else
  if Event.IsKey(CtrlX) then
  begin
    if Text <> '' then
    begin
      Clipboard.AsText := Text;
      Text := '';
      DoChange;
    end;
    Result := ExclusiveEvents;
  end else
  if Event.IsKey(CtrlV) then
  begin
    Text := SDeleteChars(Clipboard.AsText, AllChars - FAllowedChars);
    DoChange;
    Result := ExclusiveEvents;
  end else
  if (Event.EventType = itKey) and
     (Event.KeyString <> '') and
     (Event.KeyCharacter in AllowedChars) and
     ((MaxLength = 0) or (Length(Text) < MaxLength)) and
     { Do not add unprintable special characters, like Enter and Escape.
       TCastleEdit doesn't process them,
       but also users don't expect these keys are something added to the edit field.
       Note: The range CtrlA..CtrlZ includes CharEnter. }
     (not (Event.KeyCharacter in [CtrlA..CtrlZ, CharEscape])) then
  begin
    Text := Text + Event.KeyString;
    DoChange;
    Result := ExclusiveEvents;
  end;
end;

function TCastleEdit.CapturesEventsAtPosition(const Position: TVector2): boolean;
begin
  if CaptureAllInput then
    Result := true // always capture
  else
    Result := inherited CapturesEventsAtPosition(Position);
end;

procedure TCastleEdit.SetFocused(const Value: boolean);
begin
  inherited;

  if FAutoOnScreenKeyboard then
    Messaging.Send(['change-keyboard-state', TMessaging.BoolToStr(Value)])
end;

procedure TCastleEdit.SetPlaceholder(const Value: string);
begin
  if FPlaceholder <> Value then
  begin
    FPlaceholder := Value;
    VisibleChange([chRender]);
  end;
end;

procedure TCastleEdit.SetPasswordChar(const Value: Char);
begin
  if FPasswordChar <> Value then
  begin
    FPasswordChar := Value;
    VisibleChange([chRender]);
  end;
end;

{$define read_implementation_methods}
{$I auto_generated_persistent_vectors/tcastleedit_persistent_vectors.inc}
{$undef read_implementation_methods}

{$endif read_implementation}
