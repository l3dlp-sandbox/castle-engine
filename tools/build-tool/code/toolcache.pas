{
  Copyright 2022-2025 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Compilation cache operations. }
unit ToolCache;

{$I castleconf.inc}

interface

uses CastleInternalArchitectures,
  ToolCompile, ToolManifest;

procedure CacheCreate(const OverrideCompiler: TCompiler;
  const Target: TTarget; const OS: TOS; const CPU: TCPU);

procedure CacheClean;

implementation

uses SysUtils,
  CastleUtils, CastleFilesUtils, CastleTimeUtils, CastleStringUtils,
  ToolProject, ToolUtils, ToolCommonUtils, ToolAndroid, ToolIos;

procedure CacheCreate(const OverrideCompiler: TCompiler;
  const Target: TTarget; const OS: TOS; const CPU: TCPU);
var
  ProjectTemplateDir: String;

  { Make cache knowing OS, CPU (does not need to be concerned with Target). }
  procedure CacheForOsCpu(const OS: TOS; const CPU: TCPU);

    { Make cache knowing OS, CPU, Mode. }
    procedure CacheForMode(const Mode: TCompilationMode);
    const
      Compiler = coFpc;
    var
      CacheProject: TCastleProject;
      CacheProjectDir, CacheOutputPath, CachePathFull: String;
    begin
      CacheProjectDir := InclPathDelim(GetTempDir(false)) +
        'castle-engine-cache-project-' + IntToStr(Random(1000000));

      Writeln(Format('Creating compilation cache for mode "%s" using temporary dir "%s"', [
        CompilationModeToStr[Mode],
        CacheProjectDir
      ]));

      // copy project template to CacheProjectDir
      if DirectoryExists(CacheProjectDir) then
        RemoveNonEmptyDir(CacheProjectDir); // clean first
      CopyDirectory(ProjectTemplateDir, CacheProjectDir);

      // we do SetCurrentDir as this is the only way for now to set TCastleProject location
      if not SetCurrentDir(CacheProjectDir) then
        raise Exception.CreateFmt('Cannot enter project directory "%s"', [CacheProjectDir]);

      CacheProject := TCastleProject.Create;
      try
        { Quick fix for making cache for web target,
          using "castle-engine cache --target=web".

          Our web target for now doesn't create CastleAutoGenerated.pas unit,
          during normal compilation, assuming CastleAutoGenerated already
          exists in the project.
          Calling MakeCastleAutoGeneratedUnit creates it.

          TODO: Is this the proper fix? It is inconsistent with what other platforms
          do. We should either do this for all platforms, or none.
          Android, iOS and standalone do create CastleAutoGenerated.pas unit
          in castle-engine-output/...  automatically -- does this obscure explicit
          CastleAutoGenerated.pas in project? Is this OK? }
        if Target = targetWeb then
          CacheProject.MakeCastleAutoGeneratedUnit;

        CacheProject.DoCompile(Compiler, Target, OS, CPU, Mode, nil,
          { do not allow to use cache when building for cache } false);
        CacheOutputPath := CompilationOutputPath(Compiler, OS, CPU, CacheProjectDir);
      finally FreeAndNil(CacheProject) end;

      CachePathFull := CachePath +
        CPUToString(CPU) + '-' + OSToString(OS) + PathDelim +
        CompilationModeToStr[Mode] + PathDelim;
      if not ForceDirectories(CachePathFull) then
        raise Exception.CreateFmt('Cannot create directory for config file: "%s"', [CachePathFull]);

      Writeln(Format('Storing cache in "%s"', [
        CachePathFull
      ]));
      CopyDirectory(CacheOutputPath, CachePathFull);

      { change current directory to CacheProjectDir parent before trying to remove CacheProjectDir,
        Windows prevents removal otherwise. }
      {$warnings off} // using ParentPath, should be internal
      if not SetCurrentDir(ParentPath(CacheProjectDir)) then
        raise Exception.CreateFmt('Cannot enter project parent directory "%s"', [ParentPath(CacheProjectDir)]);
      {$warnings on}
      RemoveNonEmptyDir(CacheProjectDir, true);
    end;

  var
    Mode: TCompilationMode;
  begin
    for Mode := Low(Mode) to High(Mode) do
      CacheForMode(Mode);
  end;

var
  CgePath: String;
  TimeStart: TProcessTimerResult;
  Seconds: TFloatTime;
  CacheSize: QWord;
  AndroidCPU: TCPU;
begin
  TimeStart := ProcessTimer;

  if not (OverrideCompiler in [coAutodetect, coFpc]) then
    raise Exception.Create('TODO: Only caching for FPC is supported now');

  CgePath := CastleEnginePath;
  if CgePath = '' then
    raise Exception.Create(SCannotFindCgePath);
  ProjectTemplateDir := CgePath + 'tools' + PathDelim + 'build-tool' + PathDelim +
    'data' + PathDelim + 'castle_cache';

  case Target of
    targetAndroid:
      begin
        for AndroidCPU in DetectAndroidCPUS do
          CacheForOsCpu(Android, AndroidCPU);
      end;
    targetIOS:
      begin
        if IosSimulatorSupport then
        begin
          CacheForOsCpu(iphonesim, i386);
          CacheForOsCpu(iphonesim, x86_64);
        end;
        CacheForOsCpu(iOS, arm);
        CacheForOsCpu(iOS, aarch64);
      end;
    targetWeb: CacheForOsCpu(Wasip1, Wasm32);
    targetCustom: CacheForOsCpu(OS, CPU);
    else raise Exception.Create('Caching for this target is not supported now');
  end;

  CacheSize := DirectorySize(CachePath);
  Seconds := ProcessTimerSeconds(ProcessTimer, TimeStart);
  Writeln(Format('Cache created in %f seconds, size on disk: %s', [
    Seconds,
    SizeToStr(CacheSize)
  ]));
end;

procedure CacheClean;
var
  S: String;
begin
  S := CachePath;
  if DirectoryExists(S) then
  begin
    Writeln('Removing cache dir ', S);
    RemoveNonEmptyDir(S);
  end;
end;

end.
