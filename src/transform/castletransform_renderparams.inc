{%MainUnit castletransform.pas}
{
  Copyright 2010-2024 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}
{ Rendering parameters (TRenderParams and friends). }

{$ifdef read_interface}

  { Statistics about what was rendered during last frame.
    You will usually access this by viewport property,
    see @link(TCastleViewport.Statistics). }
  TRenderStatistics = record
    { How many shapes were rendered (send to OpenGL)
      versus all shapes that were potentially visible.
      Potentially visible shapes are the ones with
      TShape.Visible inside a TCastleTransform with TCastleTransform.Exists.

      When ShapesRendered is much smaller than ShapesVisible,
      it means that various algorithm(s) designed to reject invisible
      scene parts do a good job.
      This includes per-shape frustum culling (@link(TCastleScene.ShapeFrustumCulling)),
      distance culling (@link(TCastleScene.DistanceCulling)),
      occlusion culling (see @link(TCastleViewport.OcclusionCulling)). }
    ShapesRendered, ShapesVisible: Cardinal;

    { Subset of shapes counted in ShapesRendered that have been rendered using
      blending (partial transparency). }
    ShapesRenderedBlending: Cardinal;

    { The number of shapes that were not rendered,
      but their bounding box was rendered to check occlusion culling.
      This is always zero when not using occlusion culling
      (@link(TCastleViewport.OcclusionCulling)).
      Basically, this measures the "invisible overhead" of occlusion culling. }
    BoxesOcclusionQueriedCount: Cardinal;

    { TCastleScene rendered vs all visible. }
    ScenesRendered, ScenesVisible: Cardinal;

    { Draw calls done, in case of OpenGL this literally counts the number of calls
      to glDrawElements and glDrawArrays. }
    DrawCalls: Cardinal;

    { Present the statistics information as a string (without any newlines).
      Display it e.g. by assigning to TCastleLabel.Caption. }
    function ToString: String;
  end;

  PRenderStatistics = ^TRenderStatistics;

  { @exclude Internal.

    List of lights.
    Always TLightInstancesList, but we cannot declare it here as such.  }
  TAbstractLightInstancesList = TObject;

  { @exclude Internal.

    Fog node. Always TFogNode, but we cannot declare it here as such. }
  TAbstractFogNode = TObject;

  { @exclude Internal.

    Indicates rendering target for @link(TRenderingCamera.Target). }
  TRenderTarget = (
    { Normal rendering. }
    rtScreen,
    { Rendering color buffer contents to a texture inside RenderedTexture node. }
    rfRenderedTexture,
    { Rendering color buffer contents to a texture inside GeneratedCubeMap node. }
    rtCubeMapEnvironment,
    { Rendering depth buffer contents to shadow map inside GeneratedShadowMap node. }
    rtShadowMap,
    { Rendering with a special VSM shader to capture shadow map texture
      (in the normal color buffer). }
    rtVarianceShadowMap
  );

  { @exclude Internal.
    Rendering pass, for multi-pass rendering, like for shadow volumes. }
  TInternalRenderingPass = 0..1;

  { Rendering pass, for user purposes.
    Useful to keep shaders cached when you render the same scene multiple times
    in the same frame (under different lighting conditions or other things
    that change shaders). }
  TUserRenderingPass = 0..2;

  { @exclude Internal.

    Current camera used for rendering, used for @link(TRenderParams.RenderingCamera).
    This is part of TRenderParams information,
    and (like the rest of TRenderParams) it is "mostly" internal.
    If you're using the engine in a normal way, you should never need
    to deal with this class.

    This is interesting to you only if you write custom rendering code.

    In normal applications, you shoud only get/set camera using TCastleCamera
    descendants, through @link(TCastleViewport.Camera) and related properties.
    Do not use the TRenderingCamera class in normal applications. }
  TRenderingCamera = class
  strict private
    FTarget: TRenderTarget;
  public
    Camera: TCastleCamera;

    { Current camera position, direction and up.
      In the world coordinates (coordinates of whole TCastleViewport.Items). }
    View: TViewVectors;

    { Current camera matrix. Transforms from world space (normal 3D space)
      to camera space (camera space is the space where you're always
      standing on zero point, looking in -Z, and so on).

      This is needed for various things, like
      TextureCoordinateGenerator.mode = "WORLDSPACE*" or generating
      Viewpoint.camera[Inverse]Matrix event.

      Always after changing this, change also all other camera
      fields. }
    Matrix: TMatrix4;

    { Inverse of @link(Matrix).

      Always call InverseMatrixNeeded before using it,
      InverseMatrixNeeded will check InverseMatrixDone
      and eventually will calculate inverse and set InverseMatrixDone to
      @true. }
    InverseMatrix: TMatrix4;
    InverseMatrixDone: boolean;

    { Camera rotation matrix. That is, this is like @link(Matrix) but
      it doesn't move the camera, only rotates it.

      It's guaranteed that this is actually only 3x3 matrix,
      the 4th row and 4th column are all zero except the lowest right item
      which is 1.0. }
    RotationMatrix: TMatrix4;

    { Use RotationMatrix as camera matrix for rendering. }
    RotationOnly: boolean;

    { Inverse of RotationMatrix.

      Always call RotationInverseMatrixNeeded before using it,
      RotationInverseMatrixNeeded will check RotationInverseMatrixDone
      and eventually will calculate inverse and set RotationInverseMatrixDone to
      @true. }
    RotationInverseMatrix: TMatrix4;
    RotationInverseMatrixDone: boolean;

    Frustum: TFrustum;

    procedure InverseMatrixNeeded;
    procedure RotationInverseMatrixNeeded;

    { Camera rotation matrix, as a 3x3 matrix. }
    function RotationMatrix3: TMatrix3;
    function RotationInverseMatrix3: TMatrix3;

    { Set all properties (except Target)
      from TCastleCamera instance in ACamera.
      See @link(FromMatrix) for comments about @link(Target) property. }
    procedure FromCameraObject(const ACamera: TCastleCamera);

    { Set all properties (except Target) to reflect given
      camera translation (position), direction, up.
      ProjectionMatrix is needed to calculate frustum.
      Sets @link(Camera) to @nil. }
    procedure FromViewVectors(
      const ATranslation, ADirection, AUp: TVector3;
      const ProjectionMatrix: TMatrix4); overload;
    procedure FromViewVectors(
      const AViewVectors: TViewVectors;
      const ProjectionMatrix: TMatrix4); overload;

    property Target: TRenderTarget read FTarget write FTarget;

    procedure Assign(const Source: TRenderingCamera);

    { Current matrix to use for rendering. Either @link(Matrix) or @link(RotationMatrix),
      depending on @link(RotationOnly). }
    function CurrentMatrix: TMatrix4;
  end;

  { How to render a single pass, when the shapes are already collected. }
  TRenderOnePassParams = record
    { Should we use blending.

      TCastleViewport.RenderOnePass does filtering shapes based on this,
      before calling ShapesRenderer.Render.
      Matches shape that are opaque (TGLShape.UseBlending = @false)
      or transparent (TGLShape.UseBlending = @true).

      Note: When UsingBlending = @true, always DisableShadowVolumeCastingLights = @false.
      Shadow volumes simply don't allow transparent object
      to function properly as shadow receivers.
      Reading [http://developer.nvidia.com/object/fast_shadow_volumes.html]
      notes: they also just do separate rendering pass to render the
      partially-transparent parts, IOW they also note that transparent parts
      simply don't work at all with shadow volumes. }
    UsingBlending: Boolean;

    { Render only shapes that may receive shadow volumes, or ones that don't,
      or both.

      TCastleViewport.RenderOnePass does filtering shapes based on this,
      before calling ShapesRenderer.Render.
      This checks if TCastleScene.ReceiveShadowVolumes is within this set.

      Default value (after @link(Init)) is [false, true].

      Note: DisableShadowVolumeCastingLights is always @false
      when FilterShadowVolumesReceivers is only [@false]
      (to render only stuff that is never in shadow). }
    FilterShadowVolumesReceivers: TBooleanSet;

    { If @true, means that we're using multi-pass shadowing technique
      (like shadow volumes) and currently doing the "shadowed" pass.

      Which means that lights with shadowVolumes = TRUE
      (see https://castle-engine.io/shadow_volumes#_x3d_fields_shadowvolumes_and_shadowvolumesmain )
      should be turned off. }
    DisableShadowVolumeCastingLights: Boolean;

    InsideStencilTest: Boolean;

    procedure Init;
  end;

  { Callback used by @link(TRenderParams.AddRenderEvent).
    @seealso TRenderParams.AddRenderEvent }
  TRenderEvent = procedure (const Transformation: TTransformation;
    const PassParams: TRenderOnePassParams) of object;

  { Information that a TCastleTransform.LocalRender needs to render.
    "To render" means "just to collect shapes" for TCastleScene.LocalRender.
    Read-only for @link(TCastleTransform.LocalRender)
    (except @link(Statistics), which should be updated during rendering).

    @bold(This is mostly an internal class).

    You should not need to create it,
    you should not need to read anything inside or deal with this class otherwise,
    and actually you should not need to override
    @link(TCastleTransform.LocalRender) during normal engine usage.
    But it may be useful for special customized rendering. }
  TRenderParams = class
    { Rendering pass number, for internal multi-pass rendering,
      like for shadow volumes. }
    InternalPass: TInternalRenderingPass;

    { Rendering pass number, for user purposes.
      Multiplied with InternalPass to get final pass number,
      that determines shaders used. }
    UserPass: TUserRenderingPass;

    { Transformation that should be applied to the rendered result. }
    Transformation: PTransformation;

    { Current rendering statistics, should be updated by each
      @link(TCastleTransform.LocalRender) call. }
    Statistics: TRenderStatistics;

    { Fog that affects all scenes. }
    GlobalFog: TAbstractFogNode;

    { Camera frustum in local coordinates. Local for the TCastleTransform instance
      receiving this TRenderParams as @link(TCastleTransform.LocalRender)
      parameter, so it may be different from RenderingCamera.Frustum.

      As a special exception, it can be @nil, but only when TCastleScene.LocalRender
      is called (this happens from TBackgroundScene.Render).
      The TCastleScene.LocalRender should ignore frustum when it is @nil. }
    Frustum: PFrustum;

    { Are we going to render with shadow volumes, which means we have a light
      source casting shadows with shadow volumes, and renderer is capable
      (has stencil buffer etc.).

      This doesn't say at which pass are we -- collecting shapes,
      or during pass rendering unlit or lit things
      (see TRenderOnePassParams.DisableShadowVolumeCastingLights).

      This merely says we @italic(are) going to have these passes. }
    UsingShadowVolumes: Boolean;

    { Camera information for renderer. }
    RenderingCamera: TRenderingCamera;

    { Are we traversing now only inside parents with MeaningfulParent,
      i.e. outside of TCastleTransformReference.Reference. }
    InMeaningfulParent: Boolean;

    { At design-time, cameras update their own projection, to show camera gizmo.
      These are parameters to do this. }
    ProjectionBoxWithoutGizmos, ProjectionBoxWithGizmos: TBox3DEvent;
    ProjectionViewportWidth, ProjectionViewportHeight: Single;

    { Always TShapesCollector at actual rendering. }
    Collector: TObject;

    { Always TRenderer, to prepare shapes.
      This is useful to prepare shapes (rendering them with renderer
      creates VBOs, shaders, and makes them "ready" for next fast rendering).

      Note that shapes and scenes *should not* get associated with this TRenderer
      instance when being prepared.
      Shapes and scenes should remain independent of TRenderer instance,
      to easily change them to a different renderer (like a
      different TCastleViewport).
      So this TRenderer instance is provided to be used, but do not keep
      anywhere in shapes / scenes a reference to it.

      See CastleInternalRenderer for more comments how are things associated. }
    RendererToPrepareShapes: TObject;

    { Depth range that should be used for rendering. }
    DepthRange: TDepthRange;

    constructor Create;

    { Lights that shine on object (headlight and lights from all scenes with CastGlobalLights). }
    function GlobalLights: TAbstractLightInstancesList; virtual; abstract;

    { Add a custom rendering event.

      Given RenderEvent will be called multiple times during this render pass,
      with TRenderOnePassParams containing different values,
      for example once to render opaque shapes
      (with @link(TRenderOnePassParams.UsingBlending) = @false),
      once to render transparent shapes
      (with @link(TRenderOnePassParams.UsingBlending) = @true).

      So be sure to filter what you're doing inside this callback by
      @link(TRenderOnePassParams.UsingBlending) and
      @link(TRenderOnePassParams.FilterShadowVolumesReceivers). }
    procedure AddRenderEvent(const RenderEvent: TRenderEvent);

    { Camera position, in the local coordinates, after @link(Transformation). }
    function LocalCameraPosition: TVector3;
  end;

  { Information that a TCastleTransform object needs to prepare rendering.

    This is an internal class.

    You should not need to create it,
    you should not need to read anything inside or deal with this class otherwise.

    The only usage allowed, but deprecated, is to pass an instance of this class
    taken from @link(TCastleViewport.PrepareParams)
    as a parameter to @link(TCastleTransform.PrepareResources).
    You should treat this class as a "black box" in normal applications. }
  TPrepareParams = class
    { Include a headlight and all lights that shine on all
      scenes (thanks to @link(TCastleScene.CastGlobalLights)).

      It is not necessary to define this (it may be @nil).
      And all the lighting is dynamic, so of course you can always
      turn on / off things like a headlight during the game.
      However, passing here the appropriate lights will mean that the shaders
      are immediately prepared for the current lighting. }
    GlobalLights: TAbstractLightInstancesList;

    { World fog, in any, to prepare for. }
    GlobalFog: TAbstractFogNode;

    { Always TRenderer, to prepare shapes.
      This is useful to prepare shapes (rendering them with renderer
      creates VBOs, shaders, and makes them "ready" for next fast rendering).

      Note that shapes and scenes *should not* get associated with this TRenderer
      instance when being prepared.
      See @link(TRenderParams.RendererToPrepareShapes). }
    RendererToPrepareShapes: TObject;
  end;

{$endif read_interface}

{$ifdef read_implementation}

{ TRenderStatistics ---------------------------------------------------------- }

function TRenderStatistics.ToString: String;
var
  OcclusionStats: String;
begin
  if BoxesOcclusionQueriedCount <> 0 then
    OcclusionStats := Format(' (+ %d boxes for occlusion query)', [
      BoxesOcclusionQueriedCount
    ])
  else
    OcclusionStats := '';

  Result := Format('Shapes %d (with blending: %d)%s / %d, Scenes: %d / %d, Draw calls: %d', [
    ShapesRendered,
    ShapesRenderedBlending,
    OcclusionStats,
    ShapesVisible,
    ScenesRendered,
    ScenesVisible,
    DrawCalls
  ]);
end;

{ TRenderingCamera --------------------------------------------------------------- }

procedure TRenderingCamera.InverseMatrixNeeded;
begin
  if not InverseMatrixDone then
  begin
    if not Matrix.TryInverse(InverseMatrix) then
    begin
      InverseMatrix := TMatrix4.Identity;
      WritelnLogMultiline('Camera', 'Camera matrix cannot be inverted, conversions between world and camera space will not be done. Camera matrix is: ' +
        Matrix.ToRawString('  '));
    end;
    InverseMatrixDone := true;
  end;
end;

procedure TRenderingCamera.RotationInverseMatrixNeeded;
begin
  if not RotationInverseMatrixDone then
  begin
    if not RotationMatrix.TryInverse(RotationInverseMatrix) then
    begin
      RotationInverseMatrix := TMatrix4.Identity;
      WritelnLogMultiline('Camera', 'Camera rotation matrix cannot be inverted, conversions between world and camera space will not be done. Camera matrix is: ' +
        RotationMatrix.ToRawString('  '));
    end;
    RotationInverseMatrixDone := true;
  end;
end;

function TRenderingCamera.RotationMatrix3: TMatrix3;
begin
  Move(RotationMatrix.Data[0], Result.Data[0], SizeOf(Single) * 3);
  Move(RotationMatrix.Data[1], Result.Data[1], SizeOf(Single) * 3);
  Move(RotationMatrix.Data[2], Result.Data[2], SizeOf(Single) * 3);
end;

function TRenderingCamera.RotationInverseMatrix3: TMatrix3;
begin
  Move(RotationInverseMatrix.Data[0], Result.Data[0], SizeOf(Single) * 3);
  Move(RotationInverseMatrix.Data[1], Result.Data[1], SizeOf(Single) * 3);
  Move(RotationInverseMatrix.Data[2], Result.Data[2], SizeOf(Single) * 3);
end;

procedure TRenderingCamera.FromCameraObject(const ACamera: TCastleCamera);
begin
  Camera := ACamera;
  View := ACamera.WorldView;
  Matrix := ACamera.Matrix;
  InverseMatrixDone := false;
  RotationMatrix := ACamera.RotationMatrix;
  RotationInverseMatrixDone := false;
  Frustum := ACamera.Frustum;
end;

procedure TRenderingCamera.FromViewVectors(
  const ATranslation, ADirection, AUp: TVector3;
  const ProjectionMatrix: TMatrix4);
var
  V: TViewVectors;
begin
  V.Translation := ATranslation;
  V.Direction := ADirection;
  V.Up := AUp;
  FromViewVectors(V, ProjectionMatrix);
end;

procedure TRenderingCamera.FromViewVectors(
  const AViewVectors: TViewVectors;
  const ProjectionMatrix: TMatrix4);
begin
  Camera := nil;
  View := AViewVectors;
  Matrix := View.Matrix;
  InverseMatrixDone := false;
  RotationMatrix := View.RotationMatrix;
  RotationInverseMatrixDone := false;
  Frustum.Init(ProjectionMatrix, Matrix);
end;

procedure TRenderingCamera.Assign(const Source: TRenderingCamera);
begin
  FTarget                   := Source.FTarget                  ;
  Matrix                    := Source.Matrix                   ;
  InverseMatrix             := Source.InverseMatrix            ;
  InverseMatrixDone         := Source.InverseMatrixDone        ;
  RotationMatrix            := Source.RotationMatrix           ;
  RotationOnly              := Source.RotationOnly             ;
  RotationInverseMatrix     := Source.RotationInverseMatrix    ;
  RotationInverseMatrixDone := Source.RotationInverseMatrixDone;
  Frustum                   := Source.Frustum                  ;
end;

function TRenderingCamera.CurrentMatrix: TMatrix4;
begin
  if RotationOnly then
    Result := RotationMatrix
  else
    Result := Matrix;
end;

{ TRenderParams -------------------------------------------------------------- }

var
  GlobalIdentityTransform: TTransformation;

constructor TRenderParams.Create;
begin
  inherited;
  Transformation := @GlobalIdentityTransform;
  InMeaningfulParent := true;
end;

procedure TRenderParams.AddRenderEvent(const RenderEvent: TRenderEvent);
begin
  (Collector as TShapesCollector).AddRenderEvent(Transformation^, RenderEvent);
end;

function TRenderParams.LocalCameraPosition: TVector3;
begin
  Result := Transformation^.InverseTransform.MultPoint(
    RenderingCamera.View.Translation);
end;

{ TRenderOnePassParams ------------------------------------------------------- }

procedure TRenderOnePassParams.Init;
begin
  FillChar(Self, SizeOf(Self), 0);
  FilterShadowVolumesReceivers := [false, true];
end;

{$endif read_implementation}
