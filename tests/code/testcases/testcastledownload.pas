﻿// -*- compile-command: "./test_single_testcase.sh TTestDownload" -*-
{
  Copyright 2020-2025 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Test CastleDownload unit. }
unit TestCastleDownload;

interface

uses
  Classes, SysUtils, CastleTester;

type
  TTestDownload = class(TCastleTestCase)
  published
    procedure TestLocalCharsCastleData;
    procedure TestLocalCharsCastleConfig;
    procedure TesTCastleTextReader;
    procedure TestRegisteredProtocolNotCaseSensitive;
  end;

implementation

uses CastleDownload, CastleClassUtils, CastleVectors, CastleStringUtils,
  CastleFonts, CastleFilesUtils, CastleUriUtils;

procedure TTestDownload.TestLocalCharsCastleData;

  { Test reading file using URL (through CGE function). }
  procedure TestReading(const URL: String);
  var
    Stream: TStream;
    S: String;
  begin
    Stream := Download(URL);
    try
      S := StreamToString(Stream);
      AssertEquals('Testing.', Trim(S));
    finally FreeAndNil(Stream) end;
  end;

  { Test reading file, whose URL is written inside another file, in UTF-8 encoding. }
  procedure TestReadingThroughReference(const URL: String);
  var
    Stream: TStream;
    ReferredURL: String;
  begin
    Stream := Download(URL);
    try
      ReferredURL := Trim(StreamToString(Stream));
      TestReading(ReferredURL);
    finally FreeAndNil(Stream) end;
  end;

  { Test reading font (as it goes through FreeType library). }
  procedure TestReadingFont(const FontUrl: String);
  var
    MyNewFont: TCastleFont;
  begin
    MyNewFont := TCastleFont.Create(nil);
    try
      MyNewFont.Size := 20;
      MyNewFont.AntiAliased := true;
      MyNewFont.Url := FontUrl;
    finally FreeAndNil(MyNewFont) end;
  end;

begin
  TestReading('castle-data:/local_chars/ascii_name.txt');
  TestReading('castle-data:/local_chars/' + UrlEncode('name with Polish chars ćma źrebak żmija wąż królik.txt'));
  TestReading('castle-data:/local_chars/' + UrlEncode('name with Chinese chars 样例中文文本.txt'));
  TestReading('castle-data:/local_chars/' + UrlEncode('样例中文文本/name with Chinese chars 样例中文文本.txt'));
  TestReading('castle-data:/local_chars/' + UrlEncode('name with Russian chars образец русского текста.txt'));
  TestReading('castle-data:/local_chars/' + UrlEncode('образец русского текста/name with Russian chars образец русского текста.txt'));

  // Not really correct URLs, as space should be encoded as %20 etc., but we handle them too
  TestReading('castle-data:/local_chars/name with Polish chars ćma źrebak żmija wąż królik.txt');
  TestReading('castle-data:/local_chars/name with Chinese chars 样例中文文本.txt');
  TestReading('castle-data:/local_chars/样例中文文本/name with Chinese chars 样例中文文本.txt');
  TestReading('castle-data:/local_chars/name with Russian chars образец русского текста.txt');
  TestReading('castle-data:/local_chars/образец русского текста/name with Russian chars образец русского текста.txt');

  TestReadingThroughReference('castle-data:/' + UrlEncode('local_chars/reference to file with Chinese chars.txt'));
  TestReadingThroughReference('castle-data:/' + UrlEncode('local_chars/reference to file with Russian chars.txt'));
  TestReadingThroughReference('castle-data:/' + UrlEncode('local_chars/reference to file with Polish chars.txt'));

  // Not really correct URLs, as space should be encoded as %20 etc., but we handle them too
  TestReadingThroughReference('castle-data:/local_chars/reference to file with Chinese chars.txt');
  TestReadingThroughReference('castle-data:/local_chars/reference to file with Russian chars.txt');
  TestReadingThroughReference('castle-data:/local_chars/reference to file with Polish chars.txt');

  TestReadingFont('castle-data:/' + UrlEncode('local_chars/DejaVuSans name with Russian chars образец русского текста.ttf'));

  // Not really correct URLs, as space should be encoded as %20 etc., but we handle them too
  TestReadingFont('castle-data:/local_chars/DejaVuSans name with Russian chars образец русского текста.ttf');
end;

procedure TTestDownload.TestLocalCharsCastleConfig;
begin
  if not CanUseCastleConfig then
  begin
    AbortTest;
    Exit;
  end;

  StringToFile('castle-config:/' + UrlEncode('config_ascii.txt'), 'Testing save.');
  StringToFile('castle-config:/' + UrlEncode('config with Chinese chars 样例中文文本.txt'), 'Testing save.');
  StringToFile('castle-config:/' + UrlEncode('config with Polish chars ćma źrebak żmija wąż królik.txt'), 'Testing save.');
  StringToFile('castle-config:/' + UrlEncode('config with Russian chars образец русского текста.txt'), 'Testing save.');

  // Not really correct URLs, as space should be encoded as %20 etc., but we handle them too
  StringToFile('castle-config:/2_config with Chinese chars 样例中文文本.txt', 'Testing save.');
  StringToFile('castle-config:/2_config with Polish chars ćma źrebak żmija wąż królik.txt', 'Testing save.');
  StringToFile('castle-config:/2_config with Russian chars образец русского текста.txt', 'Testing save.');
end;

procedure TTestDownload.TesTCastleTextReader;

{ Testcase based on example from
  https://forum.castle-engine.io/t/setup-files-and-working-with-them/630/4
}

var
  T: TCastleTextReader;
  X, Y, Z: Single;
  V: TVector3;
begin
  { using ReadSingle }
  T := TCastleTextReader.Create('castle-data:/test_text_reader.txt');
  try
    X := T.ReadSingle;
    Y := T.ReadSingle;
    Z := T.ReadSingle;
    AssertSameValue(1, X);
    AssertSameValue(2, Y);
    AssertSameValue(3, Z);

    X := T.ReadSingle;
    Y := T.ReadSingle;
    Z := T.ReadSingle;
    AssertSameValue(4, X);
    AssertSameValue(5, Y);
    AssertSameValue(6, Z);

    X := T.ReadSingle;
    Y := T.ReadSingle;
    Z := T.ReadSingle;
    AssertSameValue(7, X);
    AssertSameValue(8, Y);
    AssertSameValue(9, Z);
  finally FreeAndNil(T) end;

  { alternative version using ReadVector3 }
  T := TCastleTextReader.Create('castle-data:/test_text_reader.txt');
  try
    V := T.ReadVector3;
    AssertVectorEquals(Vector3(1, 2, 3), V);
    V := T.ReadVector3;
    AssertVectorEquals(Vector3(4, 5, 6), V);
    V := T.ReadVector3;
    AssertVectorEquals(Vector3(7, 8, 9), V);
  finally FreeAndNil(T) end;

  { alternative version using Readln + Vector3FromStr }
  T := TCastleTextReader.Create('castle-data:/test_text_reader.txt');
  try
    V := Vector3FromStr(T.Readln);
    AssertVectorEquals(Vector3(1, 2, 3), V);
    V := Vector3FromStr(T.Readln);
    AssertVectorEquals(Vector3(4, 5, 6), V);
    V := Vector3FromStr(T.Readln);
    AssertVectorEquals(Vector3(7, 8, 9), V);
  finally FreeAndNil(T) end;

  { alternative version using Readln + DeFormat }
  T := TCastleTextReader.Create('castle-data:/test_text_reader.txt');
  try
    DeFormat(T.Readln, '%.single. %.single. %.single.', [@X, @Y, @Z]);
    AssertSameValue(1, X);
    AssertSameValue(2, Y);
    AssertSameValue(3, Z);

    DeFormat(T.Readln, '%.single. %.single. %.single.', [@X, @Y, @Z]);
    AssertSameValue(4, X);
    AssertSameValue(5, Y);
    AssertSameValue(6, Z);

    DeFormat(T.Readln, '%.single. %.single. %.single.', [@X, @Y, @Z]);
    AssertSameValue(7, X);
    AssertSameValue(8, Y);
    AssertSameValue(9, Z);
  finally FreeAndNil(T) end;
end;

procedure TTestDownload.TestRegisteredProtocolNotCaseSensitive;
begin
  AssertFalse(RegisteredUrlProtocol('my-test-proto'));
  AssertFalse(RegisteredUrlProtocol('MY-test-PROTO'));
  AssertFalse(RegisteredUrlProtocol('my-Test-proto'));

  RegisterUrlProtocol('my-Test-proto', nil, nil);

  AssertTrue(RegisteredUrlProtocol('my-test-proto'));
  AssertTrue(RegisteredUrlProtocol('MY-test-PROTO'));
  AssertTrue(RegisteredUrlProtocol('my-Test-proto'));

  UnregisterUrlProtocol('My-Test-protO'); // different case when registered, no problem

  AssertFalse(RegisteredUrlProtocol('my-test-proto'));
  AssertFalse(RegisteredUrlProtocol('MY-test-PROTO'));
  AssertFalse(RegisteredUrlProtocol('my-Test-proto'));
end;

initialization
  RegisterTest(TTestDownload);
end.
