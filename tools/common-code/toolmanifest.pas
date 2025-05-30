{
  Copyright 2021-2024 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Parsing of CastleEngineManifest.xml files (shared by CGE build tool and CGE editor).
  The central class in @link(TCastleManifest). }
unit ToolManifest;

{$I castleconf.inc}

interface

uses DOM, Classes, Generics.Collections,
  CastleStringUtils, CastleImages, CastleUtils, CastleFindFiles, CastleColors,
  CastleInternalTools, CastleUnicode,
  ToolServices, ToolAssocDocTypes;

type
  TCompiler = (coAutodetect, coFpc, coDelphi);

  TScreenOrientation = (soAny, soLandscape, soPortrait);

  TLocalizedAppName = class
    Language: String;
    AppName: String;
    constructor Create(const ALanguage, AAppName: String);
  end;
  TLocalizedAppNameList = {$ifdef FPC}specialize{$endif} TObjectList<TLocalizedAppName>;

  TIncludePath = class
    Path: String;
    Recursive: Boolean;
    ExecutablePermission: Boolean;
  end;
  TIncludePathList = {$ifdef FPC}specialize{$endif} TObjectList<TIncludePath>;

  TProjectVersion = class(TComponent)
  public
    DisplayValue: String;
    Code: Cardinal;
    { Version components separated by dots in DisplayValue:
      Major, Minor, Release, Build.
      Calculated by InitializeItems. }
    Items: array [0..3] of Cardinal;
    procedure InitializeItems;
  end;

  TImageFileNames = class(TCastleStringList)
  private
    FBaseUrl: String;
  public
    property BaseUrl: String read FBaseUrl write FBaseUrl;
    { Find image with given extension, or '' if not found.
      Note that the returned image filename is relative to project path,
      usually you should process it like @code(IconPath := CombinePaths(Project.Path, IconPath)). }
    function FindExtension(const Extensions: array of string): String;
    { Find and read an image format that we can process with our CastleImages.
      Try to read it to a class that supports nice-quality resizing (TResizeInterpolationFpImage).
      @nil if not found. }
    function FindReadable: TCastleImage;
  end;

  TLaunchImageStoryboard = class
    BaseUrl, Path: String;
    Scale: Single;
    BackgroundColor: TCastleColor;
    constructor Create;
  end;

  { Parsing of CastleEngineManifest.xml files. }
  TCastleManifest = class
  strict private
    const
      { Google Play requires version code to be >= 1 }
      DefautVersionCode = 1;
      { iOS requires version display to be <> '' }
      DefautVersionDisplayValue = '0.1';
      DefaultUsesNonExemptEncryption = true;
      DefaultDataExists = true;
      DefaultFullscreenImmersive = true;
      DefaultDetectMemoryLeaks = false;
      DefaultMacAppBundle = true;
      DefaultWebCanvasWidth = 960;
      DefaultWebCanvasHeight = 540;
      DefaultProposedUnitPrefix = 'Game';

      { character sets }
      ControlChars = [#0 .. Chr(Ord(' ') - 1)];
      AlphaNum = ['a'..'z', 'A'..'Z', '0'..'9'];

      { qualified_name is also a Java package name for Android, so it
        cannot contain dash character.

        As for underscore:
        On Android, using _ is allowed,
        but on iOS it is not (it fails at signing),
        possibly because _ inside URLs is (in general) not allowed:
        http://stackoverflow.com/questions/2180465/can-domain-name-subdomains-have-an-underscore-in-it }
      QualifiedNameAllowedChars = AlphaNum + ['.'];

    var
      OwnerComponent: TComponent;
      FName, FExecutableName, FQualifiedName, FAuthor, FCaption: String;
      FProposedUnitPrefix: String;
      FIOSOverrideQualifiedName: String;
      FIOSOverrideVersion: TProjectVersion; //< nil if not overridden, should use FVersion then
      FUsesNonExemptEncryption: boolean;
      FDataExists: Boolean;
      FPath, FPathUrl, FDataPath, FDataPathUrl: String;
      FIncludePaths: TIncludePathList;
      FExcludePaths: TCastleStringList;
      FExtraCompilerOptions, FExtraCompilerOptionsAbsolute: TCastleStringList;
      FDefines: TCastleStringList;
      FIcons, FLaunchImages: TImageFileNames;
      FLaunchImageStoryboard: TLaunchImageStoryboard;
      FSearchPaths, FLibraryPaths: TStringList;
      FStandaloneSource, FAndroidSource, FIOSSource, FPluginSource: String;
      FCompiler: TCompiler;
      FLazarusProject, FDelphiProject: String;
      FBuildUsingLazbuild: Boolean;
      FGameUnits, FEditorUnits: String;
      FVersion: TProjectVersion;
      FFullscreenImmersive: boolean;
      FScreenOrientation: TScreenOrientation;
      FAndroidCompileSdkVersion, FAndroidMinSdkVersion, FAndroidTargetSdkVersion: Cardinal;
      FAndroidServices, FIOSServices: TServiceList;
      FAssociateDocumentTypes: TAssociatedDocTypeList;
      FLocalizedAppNames: TLocalizedAppNameList;
      FIOSTeam: String;
      FindPascalFilesResult: TStringList; // valid only during FindPascalFilesCallback
      FDebianMenuSection: String;
      FDebianControlSection: String;
      FFreeDesktopCategories: String;
      FFreeDesktopComment: String;
      FDetectMemoryLeaks: Boolean;
      FMacAppBundle: Boolean;
      FProjectDependencies: TProjectDependencies;
      FWebCanvasWidth, FWebCanvasHeight: Integer;
      FWebHtmlContents: String;

    function DefaultQualifiedName(const AName: String): String;
    procedure CheckMatches(const Name, Value: String; const AllowedChars: TSetOfChars);
    class procedure CheckUnicodeDoesNotContain(
      const Name, Value: String; const DisallowedChars: TUnicodeCharList);
    procedure CheckValidQualifiedName(const OptionName: String; const QualifiedName: String);
    { Change compiler option @xxx to use absolute paths.
      Important for "castle-engine editor" where ExtraCompilerOptionsAbsolute is inserted
      into lpk, but lpk is in a different directory.
      Testcase: unholy_society. }
    function MakeAbsoluteCompilerOption(const Option: String): String;
    { Create and read version from given DOM element.
      Returns @nil if Element is @nil. }
    function ReadVersion(const Element: TDOMElement): TProjectVersion;
    procedure CreateFinish;
    procedure FindPascalFilesCallback(const FileInfo: TFileInfo; var StopSearch: boolean);
    procedure SetBaseUrl(const Value: String);
  public
    const
      { Android SDK versions.
        See https://castle-engine.io/project_manifest#_android_information . }
      DefaultAndroidCompileSdkVersion = 34;
      DefaultAndroidTargetSdkVersion = DefaultAndroidCompileSdkVersion;
      { See https://castle-engine.io/android_faq
        for reasons behind this minimal version. }
      DefaultAndroidMinSdkVersion = 21;

      DataName = 'data';

    { Load defaults.
      @param APath Project path, must be absolute. }
    constructor Create(const APath: String);
    { Load manifest file.
      @param ManifestUrl Full URL to CastleEngineManifest.xml, must be absolute. }
    constructor CreateFromUrl(const ManifestUrl: String);
    { Guess values for the manifest.
      @param APath Project path, must be absolute.
      @param AStandaloneSource Guessed StandaloneSource value. Project Name will be derived from it too. }
    constructor CreateGuess(const APath, AStandaloneSource: String);

    destructor Destroy; override;

    { Detailed information about the project, read-only and useful for
      various project operations. }
    { }

    property Version: TProjectVersion read FVersion;
    property Compiler: TCompiler read FCompiler;
    property LazarusProject: String read FLazarusProject;
    property DelphiProject: String read FDelphiProject;
    property BuildUsingLazbuild: Boolean read FBuildUsingLazbuild;
    property GameUnits: String read FGameUnits;
    property EditorUnits: String read FEditorUnits;
    property QualifiedName: String read FQualifiedName;
    property ProposedUnitPrefix: String read FProposedUnitPrefix;
    { Dependencies of this project.
      Read-only from the outside, do not call any methods that modify
      the contents of it. }
    property ProjectDependencies: TProjectDependencies read FProjectDependencies;
    { Shortcut for ProjectDependencies.Dependencies. }
    function Dependencies: TDependencies;
    property Name: String read FName;

    { Project path. Absolute.
      Always ends with path delimiter, like a slash or backslash.

      Same as @link(PathUrl) but this is a regular filename.
      Prefer to use @link(PathUrl) instead in new code, at some point this
      will be deprecated and then removed. }
    property Path: String read FPath;

    { Project path, as absolute URL.
      Always ends with slash. }
    property PathUrl: String read FPathUrl;

    property DataExists: Boolean read FDataExists;

    { Project data path. Absolute.
      Always ends with path delimiter, like a slash or backslash.
      Should be ignored if not @link(DataExists).

      Same as @link(DataPathUrl) but this is a regular filename.
      Prefer to use @link(DataPathUrl) instead in new code, at some point this
      will be deprecated and then removed.  }
    property DataPath: String read FDataPath;

    { Project data path, as absolute URL.
      Always ends with slash.
      Should be ignored if not @link(DataExists). }
    property DataPathUrl: String read FDataPathUrl;

    property Caption: String read FCaption;
    property Author: String read FAuthor;
    property ExecutableName: String read FExecutableName;
    property FullscreenImmersive: boolean read FFullscreenImmersive;
    property ScreenOrientation: TScreenOrientation read FScreenOrientation;
    property Icons: TImageFileNames read FIcons;
    property LaunchImages: TImageFileNames read FLaunchImages;
    { iOS launch image storyboard (see https://castle-engine.io/project_manifest#_launch_images_for_now_only_for_ios ).
      Never @nil (but check Path <> '' before actually using it). }
    property LaunchImageStoryboard: TLaunchImageStoryboard read FLaunchImageStoryboard;
    property SearchPaths: TStringList read FSearchPaths;
    property LibraryPaths: TStringList read FLibraryPaths;
    property AssociateDocumentTypes: TAssociatedDocTypeList read FAssociateDocumentTypes;
    property LocalizedAppNames: TLocalizedAppNameList read FLocalizedAppNames;
    property IncludePaths: TIncludePathList read FIncludePaths;
    property ExcludePaths: TCastleStringList read FExcludePaths;
    property ExtraCompilerOptions: TCastleStringList read FExtraCompilerOptions;
    property ExtraCompilerOptionsAbsolute: TCastleStringList read FExtraCompilerOptionsAbsolute;
    property Defines: TCastleStringList read FDefines;

    { iOS-specific things }
    property IOSOverrideQualifiedName: String read FIOSOverrideQualifiedName;
    property IOSOverrideVersion: TProjectVersion read FIOSOverrideVersion; //< nil if not overridden, should use FVersion then
    property UsesNonExemptEncryption: boolean read FUsesNonExemptEncryption;
    property IOSServices: TServiceList read FIOSServices;
    property IOSTeam: String read FIOSTeam;

    { Android-specific things }
    property AndroidCompileSdkVersion: Cardinal read FAndroidCompileSdkVersion;
    property AndroidMinSdkVersion: Cardinal read FAndroidMinSdkVersion;
    property AndroidTargetSdkVersion: Cardinal read FAndroidTargetSdkVersion;
    property AndroidServices: TServiceList read FAndroidServices;

    property WebCanvasWidth: Integer read FWebCanvasWidth;
    property WebCanvasHeight: Integer read FWebCanvasHeight;
    property WebHtmlContents: String read FWebHtmlContents;

    { Standalone source specified in CastleEngineManifest.xml.
      Most build tool code should use TCastleProject.StandaloneSourceFile instead,
      that can optionally auto-create the source file. }
    property StandaloneSource: String read FStandaloneSource;

    { Android source specified in CastleEngineManifest.xml.
      Most build tool code should use TCastleProject.AndroidSourceFile instead,
      that can optionally auto-create Android source file. }
    property AndroidSource: String read FAndroidSource;

    { iOS source specified in CastleEngineManifest.xml.
      Most build tool code should use TCastleProject.IOSSourceFile instead,
      that can optionally auto-create iOS source file. }
    property IOSSource: String read FIOSSource;

    { Plugin source specified in CastleEngineManifest.xml.
      Most build tool code should use TCastleProject.PluginSourceFile instead,
      that can optionally auto-create the source file. }
    property PluginSource: String read FPluginSource;

    { Debian-specific section name, for Debian control file.
      See https://www.debian.org/doc/debian-policy/ch-archive.html#s-subsections
      for allowed values.
      Used only by --package-format=deb.

      Default: 'games'. }
    property DebianControlSection: String read FDebianControlSection;

    { Debian-specific section name, for Debian menu.
      See https://www.debian.org/doc/packaging-manuals/menu.html/ch3.html#s3.5 and
      "man menufile" on Debian for more information.
      Used only by --package-format=deb.

      Default: 'Games'. }
    property DebianMenuSection: String read FDebianMenuSection;

    { Freedesktop category of the app,
      see https://www.freedesktop.org/wiki/Specifications/menu-spec/ for more info.
      Used only by --package-format=deb right now.
      Default: Game }
    property FreeDesktopCategories: String read FFreeDesktopCategories;

    { A short description of the project.
      See https://specifications.freedesktop.org/desktop-entry-spec/desktop-entry-spec-latest.html for more info.
      Used only by --package-format=deb right now. }
    property FreeDesktopComment: String read FFreeDesktopComment;

    property DetectMemoryLeaks: Boolean read FDetectMemoryLeaks;

    property MacAppBundle: Boolean read FMacAppBundle;

    { Find a file with given BaseName (contains filename, with extension, but without any path)
      among SearchPaths of this project.
      Returns absolute filename, or '' if not found. }
    // unused: function SearchFile(const BaseName: String): String;

    { Find a unit with given name among SearchPaths of this project.
      Returns absolute filename, or '' if not found.
      AUnitName is a Pascal unit name (not a filename, without any extension). }
    function SearchPascalUnit(const AUnitName: String): String;

    { Find a Pascal source file with given name among SearchPaths of this project.
      Returns absolute filename, or '' if not found.
      ABaseFileName is a filename (without any path separators) to search for. }
    function SearchPascalFile(const ABaseFileName: String): String;

    { Finds all Pascal files (units and includes -- not lpr / dpr for now).
      Returns a list with filenames relative to Path. }
    function FindPascalFiles: TStringList;

    { Convert possible manifest value of standalone_source into implied Pascal
      program name. }
    class function StandaloneSourceToProgramName(const AStandaloneSource: String): String;

    { Raise exception if AExecutableName not valid. }
    class procedure CheckExecutableName(const AExecutableName: String);
  end;

function CompilerToString(const C: TCompiler): String;
function StringToCompiler(const S: String): TCompiler;

function ScreenOrientationToString(const O: TScreenOrientation): String;
function StringToScreenOrientation(const S: String): TScreenOrientation;

const
  DefaultCompiler = coAutodetect;

implementation

uses SysUtils, Math, StrUtils,
  CastleXmlUtils, CastleFilesUtils, CastleLog, CastleUriUtils,
  ToolCommonUtils;

function CompilerToString(const C: TCompiler): String;
const
  Names: array [TCompiler] of String = (
    'autodetect',
    'fpc',
    'delphi'
  );
begin
  Result := Names[C];
end;

function StringToCompiler(const S: String): TCompiler;
begin
  for Result in TCompiler do
    if SameText(CompilerToString(Result), S) then
      Exit;
  raise Exception.CreateFmt('Invalid compiler name "%s"', [S]);
end;

{ TProjectVersion ------------------------------------------------------------- }

procedure TProjectVersion.InitializeItems;
var
  Strs: TCastleStringList;
  I: Integer;
begin
  Strs := CastleStringUtils.SplitString(DisplayValue, '.');
  try
    for I := 0 to High(Items) do
      if I < Strs.Count then
        Items[I] := StrToIntDef(Trim(Strs[I]), 0)
      else
        Items[I] := 0;
  finally FreeAndNil(Strs) end;
end;

{ TImageFileNames ------------------------------------------------------------- }

function TImageFileNames.FindExtension(const Extensions: array of string): String;
var
  I, J: Integer;
begin
  Result := '';
  for J := 0 to Length(Extensions) - 1 do
    for I := 0 to Count - 1 do
      if AnsiSameText(ExtractFileExt(Strings[I]), Extensions[J]) then
        Exit(Strings[I]);
end;

function TImageFileNames.FindReadable: TCastleImage;
var
  I: Integer;
  MimeType, Url: String;
begin
  for I := 0 to Count - 1 do
  begin
    Url := CombineUri(BaseUrl, Strings[I]);
    MimeType := UriMimeType(Url);
    if (MimeType <> '') and IsImageMimeType(MimeType, true, false) then
      Exit(LoadImage(Url, [TRGBImage, TRGBAlphaImage]));
  end;
  Result := nil;
end;

{ TLocalizedAppName ---------------------------------------------------------- }

constructor TLocalizedAppName.Create(const ALanguage, AAppName: String);
begin
  inherited Create;
  Language := ALanguage;
  AppName := AAppName;
end;

{ TLaunchImageStoryboard ----------------------------------------------------- }

constructor TLaunchImageStoryboard.Create;
begin
  inherited;
  // default values
  Scale := 1;
  BackgroundColor := Black;
end;

{ TCastleManifest ------------------------------------------------------------ }

constructor TCastleManifest.Create(const APath: String);
begin
  inherited Create;
  OwnerComponent := TComponent.Create(nil);
  FIncludePaths := TIncludePathList.Create(true);
  FExcludePaths := TCastleStringList.Create;
  FExtraCompilerOptions := TCastleStringList.Create;
  FExtraCompilerOptionsAbsolute := TCastleStringList.Create;
  FDefines := TCastleStringList.Create;
  FIcons := TImageFileNames.Create;
  FLaunchImages := TImageFileNames.Create;
  FLaunchImageStoryboard := TLaunchImageStoryboard.Create;
  FSearchPaths := TStringList.Create;
  FLibraryPaths := TStringList.Create;
  FAndroidServices := TServiceList.Create(true);
  FIOSServices := TServiceList.Create(true);
  FAssociateDocumentTypes := TAssociatedDocTypeList.Create;
  FProjectDependencies := TProjectDependencies.Create;

  { set defaults (only on fields that are not already in good default state after construction) }
  FDataExists := DefaultDataExists;
  FAndroidCompileSdkVersion := DefaultAndroidCompileSdkVersion;
  FAndroidMinSdkVersion := DefaultAndroidMinSdkVersion;
  FAndroidTargetSdkVersion := DefaultAndroidTargetSdkVersion;
  FUsesNonExemptEncryption := DefaultUsesNonExemptEncryption;
  FFullscreenImmersive := DefaultFullscreenImmersive;
  FDetectMemoryLeaks := DefaultDetectMemoryLeaks;
  FMacAppBundle := DefaultMacAppBundle;
  FWebCanvasWidth := DefaultWebCanvasWidth;
  FWebCanvasHeight := DefaultWebCanvasHeight;

  FPath := InclPathDelim(APath);
  FPathUrl := FilenameToUriSafe(FPath);
  FDataPath := InclPathDelim(FPath + DataName);
  FDataPathUrl := FilenameToUriSafe(FDataPath);
end;

constructor TCastleManifest.CreateGuess(const APath, AStandaloneSource: String);
begin
  Create(APath);

  FDataPath := InclPathDelim(Path + DataName);
  FDataPathUrl := FilenameToUriSafe(FDataPath);
  FName := DeleteFileExt(AStandaloneSource);
  FCaption := FName;
  FQualifiedName := DefaultQualifiedName(FName);
  FProposedUnitPrefix := DefaultProposedUnitPrefix;
  FExecutableName := FName;
  FCompiler := DefaultCompiler;
  FStandaloneSource := AStandaloneSource;
  FLazarusProject := FName + '.lpi';
  FDelphiProject := FName + '.dproj';
  FVersion := TProjectVersion.Create(OwnerComponent);
  FVersion.Code := DefautVersionCode;
  FVersion.DisplayValue := DefautVersionDisplayValue;
  SetBaseUrl(FilenameToUriSafe(InclPathDelim(GetCurrentDir)));

  CreateFinish;
end;

constructor TCastleManifest.CreateFromUrl(const ManifestUrl: String);

  { Get XML element attribute as Cardinal,
    using DefaultAndMinimum as default (if not exists in XML).

    Moreover, if value in XML exists but is smaller than DefaultAndMinimum,
    then we make a warning and use DefaultAndMinimum anyway. }
  function GetAttributeCardinalWithMinimum(const Element: TDOMElement;
    const AttributeName: String; const DefaultAndMinimum: Cardinal): Cardinal;
  begin
    Result := Element.AttributeCardinalDef(AttributeName, DefaultAndMinimum);
    if Result < DefaultAndMinimum then
    begin
      WritelnWarning('Manifest', Format('Value "%d" of attribute "%s.%s" in "CastleEngineManifest.xml" is smaller than default "%d", using default instead', [
        Result,
        Element.TagName,
        AttributeName,
        DefaultAndMinimum
      ]));
      Result := DefaultAndMinimum;
    end;
  end;

var
  Doc: TXMLDocument;
  ManifestFileName, AndroidProjectTypeStr: String;
  ChildElements: TXMLElementIterator;
  Element, ChildElement: TDOMElement;
  NewCompilerOption, DefaultLazarusProject, DefaultDelphiProject, NewSearchPath: String;
  IncludePath: TIncludePath;
begin
  ManifestFileName := UriToFilenameSafe(ManifestUrl);
  Create(ExtractFilePath(ManifestFileName));

  { Fix PathUrl and DataPathUrl in case ManifestUrl is not a regular file,
    e.g. when it is 'castle-config:/my-projects/foo/CastleEngineManifest.xml'
    which maps to something special with castle-editor-portable on web.
    Current Create implementation will set them to ''.
    TODO: In the future, it would be cleaner to use only URLs, never filenames,
    throughout the whole TCastleManifest code.
    Then this special fix will not be necessary. }
  if ManifestFileName = '' then
  begin
    FPathUrl := ExtractUriPath(ManifestUrl);
    FDataPathUrl := FPathUrl + DataName + '/';
    WritelnLog('ManifestUrl is not a regular file, fixing PathUrl to "%s" and DataPathUrl to "%s"', [
      FPathUrl,
      FDataPathUrl
    ]);
  end;

  SetBaseUrl(ManifestUrl);

  Doc := UrlReadXML(ManifestUrl);
  try
    Check(Doc.DocumentElement.TagName = 'project',
      'Root node of CastleEngineManifest.xml must be <project>');
    FName := Doc.DocumentElement.AttributeString('name');
    FCaption := Doc.DocumentElement.AttributeStringDef('caption', FName);
    FQualifiedName := Doc.DocumentElement.AttributeStringDef('qualified_name', DefaultQualifiedName(FName));
    FProposedUnitPrefix := Doc.DocumentElement.AttributeStringDef('proposed_unit_prefix', DefaultProposedUnitPrefix);
    FExecutableName := Doc.DocumentElement.AttributeStringDef('executable_name', FName);
    FStandaloneSource := Doc.DocumentElement.AttributeStringDef('standalone_source', '');
    FCompiler := StringToCompiler(Doc.DocumentElement.AttributeStringDef('compiler', 'autodetect'));
    if FStandaloneSource <> '' then
    begin
      DefaultLazarusProject := ChangeFileExt(FStandaloneSource, '.lpi');
      DefaultDelphiProject := ChangeFileExt(FStandaloneSource, '.dproj');
    end else
    begin
      DefaultLazarusProject := '';
      DefaultDelphiProject := '';
    end;
    FLazarusProject := Doc.DocumentElement.AttributeStringDef('lazarus_project', DefaultLazarusProject);
    FDelphiProject := Doc.DocumentElement.AttributeStringDef('delphi_project', DefaultDelphiProject);
    FAndroidSource := Doc.DocumentElement.AttributeStringDef('android_source', '');
    FIOSSource := Doc.DocumentElement.AttributeStringDef('ios_source', '');
    FPluginSource := Doc.DocumentElement.AttributeStringDef('plugin_source', '');
    FAuthor := Doc.DocumentElement.AttributeStringDef('author', '');
    FGameUnits := Doc.DocumentElement.AttributeStringDef('game_units', '');
    FEditorUnits := Doc.DocumentElement.AttributeStringDef('editor_units', '');
    FScreenOrientation := StringToScreenOrientation(
      Doc.DocumentElement.AttributeStringDef('screen_orientation', 'any'));
    FFullscreenImmersive := Doc.DocumentElement.AttributeBooleanDef('fullscreen_immersive', true);
    FBuildUsingLazbuild := Doc.DocumentElement.AttributeBooleanDef('build_using_lazbuild', false);
    FMacAppBundle := Doc.DocumentElement.AttributeBooleanDef('mac_app_bundle',
      DefaultMacAppBundle);

    FVersion := ReadVersion(Doc.DocumentElement.ChildElement('version', false));
    // create default FVersion value, if necessary
    if FVersion = nil then
    begin
      FVersion := TProjectVersion.Create(OwnerComponent);
      FVersion.Code := DefautVersionCode;
      FVersion.DisplayValue := DefautVersionDisplayValue;
    end;

    ProjectDependencies.ReadFromManifest(Doc);

    Element := Doc.DocumentElement.ChildElement('package', false);
    if Element <> nil then
    begin
      ChildElements := Element.ChildrenIterator('include');
      try
        while ChildElements.GetNext do
        begin
          ChildElement := ChildElements.Current;
          IncludePath := TIncludePath.Create;
          IncludePath.Path := ChildElement.AttributeString('path');
          IncludePath.Recursive := ChildElement.AttributeBooleanDef('recursive', false);
          IncludePath.ExecutablePermission := ChildElement.AttributeBooleanDef('executable_permission', false);
          FIncludePaths.Add(IncludePath);
        end;
      finally FreeAndNil(ChildElements) end;

      ChildElements := Element.ChildrenIterator('exclude');
      try
        while ChildElements.GetNext do
        begin
          ChildElement := ChildElements.Current;
          FExcludePaths.Add(ChildElement.AttributeString('path'));
        end;
      finally FreeAndNil(ChildElements) end;
    end;

    Element := Doc.DocumentElement.ChildElement('icons', false);
    if Element <> nil then
    begin
      ChildElements := Element.ChildrenIterator('icon');
      try
        while ChildElements.GetNext do
        begin
          ChildElement := ChildElements.Current;
          Icons.Add(ChildElement.AttributeString('path'));
        end;
      finally FreeAndNil(ChildElements) end;
    end;

    Element := Doc.DocumentElement.ChildElement('launch_images', false);
    if Element <> nil then
    begin
      ChildElements := Element.ChildrenIterator('image');
      try
        while ChildElements.GetNext do
        begin
          ChildElement := ChildElements.Current;
          LaunchImages.Add(ChildElement.AttributeString('path'));
        end;
      finally FreeAndNil(ChildElements) end;

      Element := Element.ChildElement('storyboard', false);
      if Element <> nil then
      begin
        FLaunchImageStoryboard.Path := Element.AttributeString('path');
        FLaunchImageStoryboard.Scale := Element.AttributeSingleDef('scale', 1.0);
        FLaunchImageStoryboard.BackgroundColor := Element.AttributeColorDef('background_color', Black);
        if not SameValue(FLaunchImageStoryboard.BackgroundColor[3], 1) then
          raise Exception.Create('Launch image storyboard background_color alpha must be 1.0, meaning of other values is not defined for now');
      end;
    end;

    Element := Doc.DocumentElement.ChildElement('localization', false);
    if Element <> nil then
    begin
      FLocalizedAppNames := TLocalizedAppNameList.Create(true);
      ChildElements := Element.ChildrenIterator;
      try
        while ChildElements.GetNext do
        begin
          Check(ChildElements.Current.TagName = 'caption', 'Each child of the localization node must be an <caption> element.');
          FLocalizedAppNames.Add(TLocalizedAppName.Create(
            ChildElements.Current.AttributeString('lang'),
            ChildElements.Current.AttributeString('value')));
        end;
      finally
        FreeAndNil(ChildElements);
      end;
    end;

    FAndroidCompileSdkVersion := DefaultAndroidCompileSdkVersion;
    FAndroidMinSdkVersion := DefaultAndroidMinSdkVersion;
    FAndroidTargetSdkVersion := DefaultAndroidTargetSdkVersion;
    Element := Doc.DocumentElement.ChildElement('android', false);

    if Element <> nil then
    begin
      FAndroidCompileSdkVersion := GetAttributeCardinalWithMinimum(Element, 'compile_sdk_version', DefaultAndroidCompileSdkVersion);
      FAndroidMinSdkVersion := GetAttributeCardinalWithMinimum(Element, 'min_sdk_version', DefaultAndroidMinSdkVersion);
      FAndroidTargetSdkVersion := GetAttributeCardinalWithMinimum(Element, 'target_sdk_version', DefaultAndroidTargetSdkVersion);

      if Element.AttributeString('project_type', AndroidProjectTypeStr) then
      begin
        WritelnWarning('Specifying android project_type in CastleEngineManifest.xml is deprecated, all projects are "integrated" always. Remove the project_type="..." attribute from <android ...> element in CastleEngineManifest.xml.');
      end;

      ChildElement := Element.ChildElement('components', false);
      if ChildElement <> nil then
      begin
        FAndroidServices.ReadCastleEngineManifest(ChildElement);
        WritelnWarning('Android', 'The name <components> is deprecated, use <services> now to refer to Android services');
      end;
      ChildElement := Element.ChildElement('services', false);
      if ChildElement <> nil then
        FAndroidServices.ReadCastleEngineManifest(ChildElement);
    end;

    Element := Doc.DocumentElement.ChildElement('ios', false);
    FUsesNonExemptEncryption := DefaultUsesNonExemptEncryption;
    if Element <> nil then
    begin
      FIOSTeam := Element.AttributeStringDef('team', '');

      FIOSOverrideQualifiedName := Element.AttributeStringDef('override_qualified_name', '');
      if FIOSOverrideQualifiedName <> '' then
        CheckValidQualifiedName('override_qualified_name', FIOSOverrideQualifiedName);

      FIOSOverrideVersion := ReadVersion(Element.Child('override_version', false));

      FUsesNonExemptEncryption := Element.AttributeBooleanDef('uses_non_exempt_encryption',
        DefaultUsesNonExemptEncryption);

      ChildElement := Element.ChildElement('services', false);
      if ChildElement <> nil then
        FIOSServices.ReadCastleEngineManifest(ChildElement);

      ChildElement := Element.ChildElement('service', false);
      if ChildElement <> nil then
        raise Exception.Create('<service> element directly inside <ios> element in CastleEngineManifest.xml file, this is incorrect. The <service> should be within <services>, which in turn should be in <ios>');
    end;

    Element := Doc.DocumentElement.ChildElement('associate_document_types', false);
    if Element <> nil then
    begin
      FAssociateDocumentTypes.ReadCastleEngineManifest(Element);
      if FAssociateDocumentTypes.Count > 0 then
        FAndroidServices.AddService('open_associated_urls');
    end;

    Element := Doc.DocumentElement.ChildElement('compiler_options', false);
    if Element <> nil then
    begin
      FDetectMemoryLeaks := Element.AttributeBooleanDef('detect_memory_leaks', DefaultDetectMemoryLeaks);

      ChildElement := Element.ChildElement('custom_options', false);
      if ChildElement <> nil then
      begin
        ChildElements := ChildElement.ChildrenIterator('option');
        try
          while ChildElements.GetNext do
          begin
            NewCompilerOption := ChildElements.Current.TextData;
            FExtraCompilerOptions.Add(NewCompilerOption);
            FExtraCompilerOptionsAbsolute.Add(MakeAbsoluteCompilerOption(NewCompilerOption));
          end;
        finally FreeAndNil(ChildElements) end;
      end;

      ChildElement := Element.ChildElement('defines', false);
      if ChildElement <> nil then
      begin
        ChildElements := ChildElement.ChildrenIterator('define');
        try
          while ChildElements.GetNext do
            FDefines.Add(Trim(ChildElements.Current.TextData));
        finally FreeAndNil(ChildElements) end;
      end;

      ChildElement := Element.ChildElement('search_paths', false);
      if ChildElement <> nil then
      begin
        ChildElements := ChildElement.ChildrenIterator('path');
        try
          while ChildElements.GetNext do
          begin
            NewSearchPath := ChildElements.Current.AttributeString('value');
            if IsPathAbsoluteOnDrive(NewSearchPath) then
              WritelnWarning('Search path "%s" is an absolute path, it will likely not work on other systems.', [NewSearchPath]);
            FSearchPaths.Add(NewSearchPath);
          end;
        finally FreeAndNil(ChildElements) end;
      end;

      ChildElement := Element.ChildElement('library_paths', false);
      if ChildElement <> nil then
      begin
        ChildElements := ChildElement.ChildrenIterator('path');
        try
          while ChildElements.GetNext do
            FLibraryPaths.Add(ChildElements.Current.AttributeString('value'));
        finally FreeAndNil(ChildElements) end;
      end;
    end;

    Element := Doc.DocumentElement.ChildElement('data', false);
    if Element <> nil then
      FDataExists := Element.AttributeBooleanDef('exists', DefaultDataExists)
    else
      FDataExists := DefaultDataExists;

    if FAndroidServices.HasService('open_associated_urls') then
      FAndroidServices.AddService('download_urls'); // downloading is needed when opening files from web

    FDebianControlSection := 'games';
    FDebianMenuSection := 'Games';
    Element := Doc.DocumentElement.ChildElement('debian', false);
    if Element <> nil then
    begin
      FDebianControlSection := Element.AttributeStringDef('control_section', FDebianControlSection);
      FDebianMenuSection := Element.AttributeStringDef('menu_section', FDebianMenuSection);
    end;

    FFreeDesktopCategories := 'Game';
    FFreeDesktopComment := '';
    Element := Doc.DocumentElement.ChildElement('free_desktop', false);
    if Element <> nil then
    begin
      FFreeDesktopCategories := Element.AttributeStringDef('categories', FFreeDesktopCategories);
      FFreeDesktopComment := Element.AttributeStringDef('comment', FFreeDesktopComment);
    end;

    Element := Doc.DocumentElement.ChildElement('web', false);
    if Element <> nil then
    begin
      FWebHtmlContents := Element.AttributeStringDef('html_contents', '');
      ChildElement := Element.ChildElement('canvas', false);
      if ChildElement <> nil then
      begin
        FWebCanvasWidth := ChildElement.AttributeIntegerDef('width', DefaultWebCanvasWidth);
        FWebCanvasHeight := ChildElement.AttributeIntegerDef('height', DefaultWebCanvasHeight);
      end;
    end;
  finally FreeAndNil(Doc) end;

  CreateFinish;
end;

destructor TCastleManifest.Destroy;
begin
  FreeAndNil(FProjectDependencies);
  FreeAndNil(OwnerComponent);
  FreeAndNil(FIncludePaths);
  FreeAndNil(FExcludePaths);
  FreeAndNil(FExtraCompilerOptions);
  FreeAndNil(FExtraCompilerOptionsAbsolute);
  FreeAndNil(FDefines);
  FreeAndNil(FIcons);
  FreeAndNil(FLaunchImages);
  FreeAndNil(FLaunchImageStoryboard);
  FreeAndNil(FSearchPaths);
  FreeAndNil(FLibraryPaths);
  FreeAndNil(FAndroidServices);
  FreeAndNil(FIOSServices);
  FreeAndNil(FAssociateDocumentTypes);
  FreeAndNil(FLocalizedAppNames);
  inherited;
end;

procedure TCastleManifest.SetBaseUrl(const Value: String);
begin
  Icons.BaseUrl := Value;
  LaunchImages.BaseUrl := Value;
  LaunchImageStoryboard.BaseUrl := Value;
end;

function TCastleManifest.DefaultQualifiedName(const AName: String): String;
begin
  Result := SDeleteChars(FName, AllChars - QualifiedNameAllowedChars);
  { On Android, package name cannot be just a word, it must have some dot. }
  if Pos('.', Result) = 0 then
    Result := 'com.mycompany.' + Result;
end;

procedure TCastleManifest.CheckMatches(const Name, Value: String; const AllowedChars: TSetOfChars);
var
  I: Integer;
begin
  for I := 1 to Length(Value) do
    if not (Value[I] in AllowedChars) then
      raise Exception.CreateFmt('Project %s contains invalid characters: "%s", this character is not allowed: "%s"',
        [Name, Value, SReadableForm(Value[I])]);
end;

class procedure TCastleManifest.CheckUnicodeDoesNotContain(
  const Name, Value: String; const DisallowedChars: TUnicodeCharList);
var
  Iter: TCastleStringIterator;
begin
  Iter.Start(Value);
  while Iter.GetNext do
  begin
    if DisallowedChars.IndexOf(Iter.Current) <> -1 then
      raise Exception.CreateFmt('Project %s contains invalid characters: "%s", this character is not allowed: "%s"', [
        Name,
        Value,
        UnicodeCharToReadableString(Iter.Current)
      ]);
  end;
end;

class procedure TCastleManifest.CheckExecutableName(const AExecutableName: String);
var
  DisallowedChars: TUnicodeCharList;
  DisallowedChar: TUnicodeChar;
begin
  { Executable name can contain everything that is an allowed filename
    on modern platforms.
    See https://superuser.com/questions/358855/what-characters-are-safe-in-cross-platform-file-names-for-linux-windows-and-os .
    In particular, most local (Chinese, Polish...) characters are OK. }
  DisallowedChars := TUnicodeCharList.Create;
  try
    DisallowedChars.Add('\/:*?"<>|');
    for DisallowedChar := 0 to 31 do // disallow null, ASCII control characters
      DisallowedChars.Add(DisallowedChar);
    CheckUnicodeDoesNotContain('executable_name', AExecutableName, DisallowedChars);
  finally FreeAndNil(DisallowedChars) end;
end;

procedure TCastleManifest.CheckValidQualifiedName(const OptionName: String; const QualifiedName: String);
var
  Components: TStringList;
  I: Integer;
begin
  CheckMatches(OptionName, QualifiedName, QualifiedNameAllowedChars);

  if (QualifiedName <> '') and
     ((QualifiedName[1] = '.') or
      (QualifiedName[Length(QualifiedName)] = '.')) then
    raise Exception.CreateFmt('%s (in %s) cannot start or end with a dot: "%s"', [
      OptionName,
      ManifestName,
      QualifiedName
    ]);

  Components := CastleStringUtils.SplitString(QualifiedName, '.');
  try
    for I := 0 to Components.Count - 1 do
    begin
      if Components[I] = '' then
        raise Exception.CreateFmt('%s (in %s) must contain a number of non-empty components separated with dots: "%s"', [
          OptionName,
          ManifestName,
          QualifiedName
        ]);
      if Components[I][1] in ['0'..'9'] then
        raise Exception.CreateFmt('%s (in %s) components must not start with a digit: "%s"', [
          OptionName,
          ManifestName,
          QualifiedName
        ]);
    end;
  finally FreeAndNil(Components) end;
end;

function TCastleManifest.ReadVersion(const Element: TDOMElement): TProjectVersion;
begin
  if Element = nil then
    Exit(nil);
  Result := TProjectVersion.Create(OwnerComponent);
  Result.DisplayValue := Element.AttributeString('value');
  CheckMatches('version value', Result.DisplayValue, AlphaNum + ['_','-','.']);
  Result.Code := Element.AttributeCardinalDef('code', DefautVersionCode);
end;

function TCastleManifest.MakeAbsoluteCompilerOption(const Option: String): String;
begin
  Result := Trim(Option);
  if (Length(Result) >= 2) and (Result[1] = '@') then
    Result := '@' + CombinePaths(Path, SEnding(Result, 2));
end;

procedure TCastleManifest.CreateFinish;

  { If DataExists, check whether DataPath really exists.
    If it doesn't exist, make a warning and set FDataExists to false. }
  procedure CheckDataExists;
  begin
    if FDataExists then
    begin
      if UriExists(DataPathUrl) = ueDirectory then
        WritelnLog('Found data in "' + DataPath + '"')
      else
      begin
        WritelnWarning('Data directory not found (tried "' + DataPathUrl + '"). If this project has no data, add <data exists="false"/> to CastleEngineManifest.xml.');
        FDataExists := false;
      end;
    end else
    begin
      if UriExists(DataPathUrl) = ueDirectory then
        WritelnWarning('Possible data directory found in "' + DataPathUrl + '", but your project has <data exists="false"/> in CastleEngineManifest.xml, so it will be ignored.' + NL +
        '  To remove this warning:' + NL +
        '  1. Rename this directory to something else than "data" (if it should not be packaged),' + NL +
        '  2. Remove <data exists="false"/> from CastleEngineManifest.xml (if "data" should be packaged).');
    end;
  end;

  { Check correctness. }
  procedure CheckManifestCorrect;
  var
    ProgramName: String;
  begin
    { Note that project "name" can contain minus ("-")
      character which is not allowed inside a Pascal identifier.
      This is a deliberate feature (we like names like "castle-model-viewer").
      When we have to derive some Pascal identifier from it, we use
      MakeProjectPascalName , TCastleProject.NamePascal and related. }
    CheckMatches('name', Name, AlphaNum + ['_','-']);

    CheckExecutableName(ExecutableName);

    { non-filename stuff: allow also dots }
    CheckValidQualifiedName('qualified_name', QualifiedName);

    { more user-visible stuff, where we allow spaces, local characters and so on }
    CheckMatches('caption', Caption, AllChars - ControlChars);
    CheckMatches('author', Author  , AllChars - ControlChars);

    { StandaloneSource, if specified, determines the dpr filename
      and (sans extension) the Pascal "program" declaration
      (these 2 things have to match exactly, compilers check this when "program"
      is specified, and we have to specify "program" otherwise Delphi IDE
      breaks "uses" clause when adding units).
      As such, it has to be a valid Pascal identifier. }
    if StandaloneSource <> '' then
    begin
      ProgramName := StandaloneSourceToProgramName(StandaloneSource);
      if not IsValidIdent(ProgramName) then
        //raise Exception.CreateFmt
        WritelnWarning('Program name "%s" (determined by standalone_source "%s" in CastleEngineManifest.xml) is not a valid Pascal identifier. This will be an error in future CGE versions, please rename your DPR / LPR.', [
          ProgramName,
          StandaloneSource
        ]);
    end;

    if ProposedUnitPrefix = '' then
      raise Exception.Create('Proposed unit prefix is empty');
    if not IsValidIdent(ProposedUnitPrefix) then
      raise Exception.CreateFmt('Proposed unit prefix "%s" is not a valid Pascal identifier', [ProposedUnitPrefix]);

    if AndroidMinSdkVersion > AndroidTargetSdkVersion then
      raise Exception.CreateFmt('Android min_sdk_version %d is larger than target_sdk_version %d, this is incorrect',
        [AndroidMinSdkVersion, AndroidTargetSdkVersion]);
  end;

begin
  Version.InitializeItems;
  CheckDataExists;

  // depends on FDataExists finalized, so must be after CheckDataExists
  if DataExists then
    FProjectDependencies.GuessDependencies(DataPath);

  // must be after GuessDependencies, to close also guessed dependencies
  FProjectDependencies.CloseDependencies;

  CheckManifestCorrect; // must be at end, to validate all
end;

function TCastleManifest.Dependencies: TDependencies;
begin
  Result := FProjectDependencies.Dependencies;
end;

{
function TCastleManifest.SearchFile(const BaseName: String): String;
var
  SearchPath, FileNameAbsolute: String;
begin
  for SearchPath in SearchPaths do
  begin
    FileNameAbsolute := CombinePaths(CombinePaths(Path, SearchPath), BaseName);
    if RegularFileExists(FileNameAbsolute) then
      Exit(FileNameAbsolute);
  end;
  Result := '';
end;
}

function TCastleManifest.SearchPascalUnit(const AUnitName: String): String;

  function SearchInPath(const SearchPathAbsolute: String): String;
  var
    FileNameAbsolute: String;
  begin
    FileNameAbsolute := CombinePaths(SearchPathAbsolute, AUnitName + '.pas');
    if RegularFileExists(FileNameAbsolute) then
      Exit(FileNameAbsolute);

    FileNameAbsolute := CombinePaths(SearchPathAbsolute, AUnitName + '.pp');
    if RegularFileExists(FileNameAbsolute) then
      Exit(FileNameAbsolute);

    { for case-sensitive filesystems, search also for lowercase version,
      just like FPC does. }

    FileNameAbsolute := CombinePaths(SearchPathAbsolute, LowerCase(AUnitName) + '.pas');
    if RegularFileExists(FileNameAbsolute) then
      Exit(FileNameAbsolute);

    FileNameAbsolute := CombinePaths(SearchPathAbsolute, LowerCase(AUnitName) + '.pp');
    if RegularFileExists(FileNameAbsolute) then
      Exit(FileNameAbsolute);

    Result := '';
  end;

var
  SearchPath: String;
begin
  Result := SearchInPath(Path);
  if Result <> '' then
    Exit;

  for SearchPath in SearchPaths do
  begin
    Result := SearchInPath(CombinePaths(Path, SearchPath));
    if Result <> '' then
      Exit;
  end;
end;

function TCastleManifest.SearchPascalFile(const ABaseFileName: String): String;

  function SearchInPath(const SearchPathAbsolute: String): String;
  var
    FileNameAbsolute: String;
  begin
    FileNameAbsolute := CombinePaths(SearchPathAbsolute, ABaseFileName);
    if RegularFileExists(FileNameAbsolute) then
      Exit(FileNameAbsolute);

    Result := '';
  end;

var
  SearchPath: String;
begin
  Result := SearchInPath(Path);
  if Result <> '' then
    Exit;

  for SearchPath in SearchPaths do
  begin
    Result := SearchInPath(CombinePaths(Path, SearchPath));
    if Result <> '' then
      Exit;
  end;
end;

function TCastleManifest.FindPascalFiles: TStringList;

  procedure ScanSearchPath(const SearchPath: String);
  var
    SearchPathAbsolute: String;
  begin
    SearchPathAbsolute := CombinePaths(Path, SearchPath);
    FindFiles(SearchPathAbsolute, '*.pas', false, @FindPascalFilesCallback, []);
    FindFiles(SearchPathAbsolute, '*.pp', false, @FindPascalFilesCallback, []);
    FindFiles(SearchPathAbsolute, '*.inc', false, @FindPascalFilesCallback, []);
  end;

var
  SearchPath: String;
begin
  Result := TStringList.Create;
  FindPascalFilesResult := Result;

  ScanSearchPath(''); // the project root is implicitly always on search path
  for SearchPath in SearchPaths do
    ScanSearchPath(SearchPath);

  FindPascalFilesResult := nil; // nothing should use it, but don't leave dangling pointer anyway
end;

procedure TCastleManifest.FindPascalFilesCallback(
  const FileInfo: TFileInfo; var StopSearch: boolean);
var
  Relative: String;
begin
  Relative := ExtractRelativePath(Path, FileInfo.AbsoluteName);
  {$ifdef MSWINDOWS}
  { Use forward slashes also on Windows,
    this way LPI generated on Windows and Unix will look the same
    (so e.g. version control systems will not report differences). }
  StringReplaceAllVar(Relative, '\', '/');
  {$endif}
  FindPascalFilesResult.Add(Relative);
end;

class function TCastleManifest.StandaloneSourceToProgramName(const AStandaloneSource: String): String;
begin
  { Use ExtractFileName to ignore path in AStandaloneSource
    which is possible, user can specify path like "code/myprogram_standalone.lpr". }
  Result := ExtractFileName(DeleteFileExt(AStandaloneSource));
end;

{ globals -------------------------------------------------------------------- }

const
  ScreenOrientationNames: array [TScreenOrientation] of string =
  ('any', 'landscape', 'portrait');

function ScreenOrientationToString(const O: TScreenOrientation): String;
begin
  Result := ScreenOrientationNames[O];
end;

function StringToScreenOrientation(const S: String): TScreenOrientation;
begin
  for Result in TScreenOrientation do
    if AnsiSameText(ScreenOrientationNames[Result], S) then
      Exit;
  raise Exception.CreateFmt('Invalid orientation name "%s"', [S]);
end;

end.
