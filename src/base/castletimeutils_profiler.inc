{
  Copyright 2018-2018 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Part of CastleTimeUtils unit: TCastleProfiler. }

{$ifdef read_interface}

type
  { Structure obtained by calling @link(TCastleProfiler.Start). }
  TCastleProfilerTime = record
  private
    FTimeStart: TTimerResult;
    FProcessTimeStart: TProcessTimerResult;
    FTime: TObject; //< Always TCastleProfiler.TTime
  end;

  { Profiler, to measure the speed of execution of your code.
    This is usually used only through the singleton @link(Profiler).

    The profiler is automatically used by various Castle Game Engine loading
    routines. It presents the easiest way to answer questions like
    "which game asset is the most expensive to load".

    To get the profile:

    @orderedList(
      @item(
        Set @link(Enabled) to @true as early as possible,
        like in the "initialization" section of your main game unit.)

      @item(
        The engine will automatically send to log a profile after
        @link(TCastleApplication.OnInitialize) finished,
        which is a usual place when loading takes place in simple CGE applications.

        You don't need to do anything, just make sure you have logging enabled
        (call @link(InitializeLog)).)

      @item(
        You can display the profile @link(Summary) at any point in your application,
        in any way you like.
        For example by

        @longCode(#
          WritelnLogMultiline('Time Profile', Profiler.Summary);
        #)

        After displaying the profile,
        you can clear the profile afterwards by calling @link(Clear),
        if you prefer the next profile to no longer show what you already output.
      )
    )

    To send your own time information to the profile,
    call @link(Start) and @link(Stop) methods around a given time-consuming task.
    We will measure time and automatically build a tree of tasks
    (when one larger task contains several smaller tasks).

    Contrary to profilers like Valgrind or Gprof
    (see
    https://castle-engine.io/manual_optimization.php#section_profiling ,
    https://github.com/castle-engine/castle-engine/wiki/Profiling-Using-Valgrind ),
    this profiler does not automatically gather everything.
    On the other hand, this profiler works on all platforms supported by CGE,
    and is trivial to use.
  }
  TCastleProfiler = class
  private
    {$ifndef PASDOC}
    // TODO: for PasDoc: Something here confuses PasDoc, although it generally can handle nested classes.

    type
      TTimes = class;

      TTime = class
        Description: String;
        Time, ProcessTime: TFloatTime;
        Finished: Boolean;
        Children: TTimes; //< nil if empty
        destructor Destroy; override;
        function Summary(const Indent: String): String;
      end;

      TTimeList = {$ifdef CASTLE_OBJFPC}specialize{$endif} TObjectList<TTime>;

      TTimes = class(TTimeList)
        procedure Sort;
        function Summary(const Indent: String): String;
      end;

    var
      FEnabled: boolean;

      { Collected times tree. }
      Times: TTimes;

      { The currently started (but not stopped) TTime structures. }
      CurrentStack: TTimeList;

    {$endif}
  public
    constructor Create;
    destructor Destroy; override;

    { Whether to gather speed measurements.
      When not enabled, the @link(Start) and @link(Stop) methods do as little
      as possible to not waste time. }
    property Enabled: boolean read FEnabled write FEnabled;

    { Summary of the gathered speed measurements. }
    function Summary: string;

    { Clear currently gathered times.

      Do not call this when some time measure is started but not yet stopped
      Currently, this will cause such time measure to be rejected,
      and stopping it will cause a one-time warning,
      but don't depend on this exact behaviour in the future.
      We will always gracefully accept this case (not crash). }
    procedure Clear;

    { Start a task which execution time will be measured.
      You @italic(must) later call @link(Stop) with
      the returned TCastleProfilerTime value.

      Typical usage looks like this:

      @longCode(#
      procedure TMyClass.LoadSomething;
      var
        TimeStart: TCastleProfilerTime;
      begin
        TimeStart := Profiler.Start('Loading something (TMyClass)');
        try
          // do the time-consuming loading now...
        finally
          Profiler.Stop(TimeStart);
        end;
      end;
      #)

      If you don't use the "finally" clause to @italic(always) call
      the matching @link(Stop), and an exception may occur in LoadSomething,
      then you will have an unmatched Start / Stop calls.
      The profiler output is undefined in this case.
      (However, we guarantee that the profiler will not crash or otherwise cause
      problems in the application.)
      So, you do not really have to wrap this in "try ... finally ... end" clause,
      if it's acceptable that the profiler output is useless
      in exceptional situations.

      @seealso Stop
    }
    function Start(const Description: String): TCastleProfilerTime;

    { Stop a task. This call must match earlier @link(Start) call.
      @seealso Start }
    procedure Stop(const TimeStart: TCastleProfilerTime);
  end;

{ Single instance of TCastleProfiler that you can use for all profiling.
  Castle Game Engine uses it to report loading times of various assets
  automatically. }
function Profiler: TCastleProfiler;

{$endif read_interface}

{$ifdef read_implementation}

{ TCastleProfiler.TTime ------------------------------------------------------ }

function TCastleProfiler.TTime.Summary(const Indent: String): String;
begin
  if not Finished then
    Result := Indent + Format('UNFINISHED - %s', [Description]) + NL
  else
    Result := Indent + Format('%f [%f] - %s', [Time, ProcessTime, Description]) + NL;
  if Children <> nil then
    Result := Result + Children.Summary(Indent + '> ');
end;

destructor TCastleProfiler.TTime.Destroy;
begin
  FreeAndNil(Children);
  inherited;
end;

{ TCastleProfiler.TTimes ----------------------------------------------------- }

  { Helper for TTimes.Sort }
  function TimeSort(constref Value1, Value2: TCastleProfiler.TTime): Integer;
  begin
    Result := Sign(Value2.Time - Value1.Time);
  end;

procedure TCastleProfiler.TTimes.Sort;
type
  TTimeComparer = {$ifdef CASTLE_OBJFPC}specialize{$endif} TComparer<TTime>;
begin
  inherited Sort(TTimeComparer.Construct(@TimeSort));
end;

function TCastleProfiler.TTimes.Summary(const Indent: String): String;
var
  I: Integer;
begin
  Sort;
  Result := '';
  for I := 0 to Count - 1 do
    Result := Result + Items[I].Summary(Indent);
end;

{ TCastleProfiler ------------------------------------------------------------------ }

var
  FProfiler: TCastleProfiler;

function Profiler: TCastleProfiler;
begin
  if FProfiler = nil then
    FProfiler := TCastleProfiler.Create;
  Result := FProfiler;
end;

constructor TCastleProfiler.Create;
begin
  inherited;
  Times := TTimes.Create(true);
  CurrentStack := TTimeList.Create(false);
end;

destructor TCastleProfiler.Destroy;
begin
  FreeAndNil(CurrentStack);
  FreeAndNil(Times);
  inherited;
end;

function TCastleProfiler.Summary: string;
begin
  Result :=
    'Profile (speed of execution).' + NL +
    'Each line shows time, followed by [time spent in this process], followed by description.' + NL +
    'Lines are sorted within each group, to show the most expensive operation at the top.' + NL +
    NL +
    Times.Summary('');
end;

function TCastleProfiler.Start(const Description: String): TCastleProfilerTime;
var
  ParentTimes: TTimes;
  T, Last: TTime;
begin
  if not Enabled then
  begin
    Result.FTime := nil;
    Exit; // rest of Result doesn't matter
  end;

  T := TTime.Create;
  T.Description := Description;

  if CurrentStack.Count <> 0 then
  begin
    Last := CurrentStack.Last;
    if Last.Children = nil then
      Last.Children := TTimes.Create(true);
    ParentTimes := Last.Children;
  end else
    ParentTimes := Times;

  ParentTimes.Add(T);
  CurrentStack.Add(T);

  Result.FTime := T;
  Result.FTimeStart := Timer;
  Result.FProcessTimeStart := ProcessTimer;
end;

procedure TCastleProfiler.Stop(const TimeStart: TCastleProfilerTime);
var
  T: TTime;
begin
  if TimeStart.FTime = nil then
    Exit; // Start was called when Enabled = false

  T := TTime(TimeStart.FTime);

  if T.Finished then
    WritelnWarning('Called Stop on the same TCastleProfilerTime instance (%s) multiple times',
      [T.Description]);

  T.Finished := true;
  T.Time := TimeStart.FTimeStart.ElapsedTime;
  T.ProcessTime := TimeStart.FProcessTimeStart.ElapsedTime;

  if (CurrentStack.Count <> 0) and
     (CurrentStack.Last = T) then
    CurrentStack.Delete(CurrentStack.Count - 1)
  else
    WritelnWarning('Mismatched Profiler.Start/Stop calls, the currently Stopped time measure (%s) was not the last one started',
      [T.Description]);
end;

procedure TCastleProfiler.Clear;
begin
  Times.Clear;
  CurrentStack.Clear;
end;

{$endif read_implementation}
