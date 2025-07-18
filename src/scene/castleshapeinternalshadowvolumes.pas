{
  Copyright 2003-2025 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}
{ Internal data for shadow volumes rendering in shapes. }
unit CastleShapeInternalShadowVolumes;

{$I castleconf.inc}

interface

uses Generics.Collections,
  CastleUtils, CastleVectors, CastleTriangles;

type
  { Edge, manifold (between exactly two triangles) or border (otherwise).
    This is used by @link(TShapeShadowVolumes.ManifoldEdges) and
    @link(TShapeShadowVolumes.BorderEdges),
    and this is crucial for rendering silhouette shadow volumes in OpenGL. }
  TEdge = record
    { Index to get vertexes of this edge.

      During rendering, the edge is defined by these two vertexes:
      - Triangles[0][VertexIndex]
      - Triangles[0][(VertexIndex + 1) mod 3]

      These are indexes to @link(TShapeShadowVolumes.TrianglesListShadowCasters)
      in turn. This way if a shape changes during animation,
      we need to recalculate @link(TShapeShadowVolumes.TrianglesListShadowCasters),
      but the @link(TShapeShadowVolumes.ManifoldEdges) and
      @link(TShapeShadowVolumes.BorderEdges) may stay unchanged. }
    VertexIndex: Cardinal;

    { Indexes to @link(TShapeShadowVolumes.TrianglesListShadowCasters) array.

      For manifold edges, both indexes are set.

      For border edges, only the 1st (Triangles[0]) index is valid.
      We overuse now Triangles[1] to mark (using High(Cardinal)) that
      the other border edge was found. }
    Triangles: array [0..1] of Cardinal;

    { These are vertex values at VertexIndex and (VertexIndex+1)mod 3 positions,
      but @italic(only at generation of manifold edges time).
      Updating this data later would be costly.
      However, using these when generating
      makes a great speed-up when generating manifold edges.

      Memory cost is acceptable: assume we have model with 10 000 faces,
      so 15 000 edges (assuming it's correctly closed manifold), so we waste
      15 000 * 2 * SizeOf(TVector3) = 360 000 bytes... that's really nothing
      to worry (we waste much more on other things).

      Checked with "The Castle": indeed, this costs about 1 MB memory
      (out of 218 MB...), with really lot of creatures... On the other hand,
      this causes small speed-up when loading: loading creatures is about
      5 seconds faster (18 with, 23 without this).

      So memory loss is small, speed gain is noticeable (but still small),
      implementation code is a little simplified, so we're keeping this for now. }
    V0, V1: TVector3;

    function TriangleIndex: Cardinal; deprecated 'use Triangles[0] instead';
  end;
  PEdge = ^TEdge;

  TEdgeList = {$ifdef FPC}specialize{$endif} TStructList<TEdge>;

  TManifoldEdge = TEdge deprecated 'use TEdge instead';
  PManifoldEdge = PEdge deprecated 'use PEdge instead';
  TManifoldEdgeList = TEdgeList deprecated 'use TEdgeList instead';
  TBorderEdge = TEdge deprecated 'use TEdge instead';
  PBorderEdge = PEdge deprecated 'use PEdge instead';
  TBorderEdgeList = TEdgeList deprecated 'use TEdgeList instead';

  { Triangles array for shadow casting shape. In local shape coordinates. }
  TTrianglesShadowCastersList = TTriangle3List;

  TShapeShadowVolumes = class
  strict private
    type
      TValidities = set of (
        svTrianglesListShadowCasters,
        svManifoldAndBorderEdges
      );
    var
      Validities: TValidities;
      FTrianglesListShadowCasters: TTrianglesShadowCastersList;
      FManifoldEdges: TEdgeList;
      FBorderEdges: TEdgeList;
      procedure CalculateIfNeededManifoldAndBorderEdges;
  public
    FShape: TObject;

    constructor Create(const AShape: TObject);
    destructor Destroy; override;

    { Removes svManifoldAndBorderEdges from Validities,
      and clears FManifold/BordEdges variables. }
    procedure InvalidateManifoldAndBorderEdges;

    { Removes svTrianglesListShadowCasters from Validities,
      and clears FTrianglesListShadowCasters variable. }
    procedure InvalidateTrianglesListShadowCasters;

    { Returns an array of triangles that should be shadow casters
      for this scene.

      Results of these functions are cached, and are also owned by this object.
      So don't modify it, don't free it. }
    function TrianglesListShadowCasters: TTrianglesShadowCastersList;

    { ManifoldEdges is a list of edges that have exactly @bold(two) neighbor
      triangles, and BorderEdges is a list of edges that have exactly @bold(one)
      neighbor triangle. These are crucial for rendering shadows using shadow
      volumes.

      Edges with more than two neighbors are allowed. If an edge has an odd
      number of neighbors, it will be placed in BorderEdges. Every other pair
      of neighbors will be "paired" and placed as one manifold edge inside
      ManifoldEdges. So actually edge with exactly 1 neighbor (odd number,
      so makes one BorderEdges item) and edge with exactly 2 neighbors
      (even number, one pair of triangles, makes one item in ManifoldEdges)
      --- they are just a special case of a general rule, that allows any
      neighbors number.

      Note that vertexes must be consistently ordered in triangles.
      For two neighboring triangles, if one triangle's edge has
      order V0, V1, then on the neighbor triangle the order must be reversed
      (V1, V0). This is true in almost all situations, for example
      if you have a closed solid object and all outside faces are ordered
      consistently (all CCW or all CW).
      Failure to order consistently will result in edges not being "paired",
      i.e. we will not recognize that some 2 edges are in fact one edge between
      two neighboring triangles --- and this will result in more edges in
      BorderEdges.

      Both of these lists are calculated at once, i.e. when you call ManifoldEdges
      or BorderEdges for the 1st time, actually both ManifoldEdges and
      BorderEdges are calculated at once. If all edges are in ManifoldEdges,
      then the scene is a correct closed manifold, or rather it's composed
      from any number of closed manifolds.

      Results of these functions are cached, and are also owned by this object.
      So don't modify it, don't free it.

      This uses TrianglesListShadowCasters.

      @groupBegin }
    function ManifoldEdges: TEdgeList;
    function BorderEdges: TEdgeList;
    { @groupEnd }

    procedure PrepareResources;
    procedure FreeResources;
  end;

var
  { By default, CalculateDetectedWholeSceneManifold detection stops
    as soon as it knows the result is @false, so it is fast,
    but also it can leave some border edges
    with Triangles[1] = 0 (unmatched) instead of Triangles[1] = High(Cardinal)
    (matching some shapes).

    For Castle Model Viewer visualization, it's useful to force
    matching all border edges, so we can see proper blue vs white colors in
    "View -> Fill Mode -> Silhouette and Border Edges" mode. }
  WholeSceneManifoldDetectionForceAllEdges: Boolean = false;

{ Calculate proper value for TCastleSceneCore.InternalDetectedWholeSceneManifold.
  ShapeListCore must be TShapeList, but cannot be declared as such. }
function CalculateDetectedWholeSceneManifold(const ShapeListCore: TObject): Boolean;

implementation

uses SysUtils,
  CastleShapes, X3DNodes, CastleLog, CastleTransform,
  CastleRenderOptions;

{ TEdge ---------------------------------------------------------------------- }

function TEdge.TriangleIndex: Cardinal;
begin
  Result := Triangles[0];
end;

{ TTriangleAdder ------------------------------------------------------------- }

type
  TTriangleAdder = class
    TriangleList: TTriangle3List;
    procedure AddTriangle(Shape: TObject;
      const Triangle: TTriangle3;
      const Normal: TTriangle3; const TexCoord: TTriangle4;
      const Face: TFaceIndex);
  end;

procedure TTriangleAdder.AddTriangle(Shape: TObject;
  const Triangle: TTriangle3;
  const Normal: TTriangle3; const TexCoord: TTriangle4;
  const Face: TFaceIndex);
begin
  if Triangle.IsValid then
    TriangleList.Add(Triangle);
end;

{ TShapeShadowVolumes -------------------------------------------------------- }

constructor TShapeShadowVolumes.Create(const AShape: TObject);
begin
  inherited Create;
  FShape := AShape;
end;

destructor TShapeShadowVolumes.Destroy;
begin
  { free FTrianglesList* variables }
  InvalidateTrianglesListShadowCasters;
  { frees FManifoldEdges, FBorderEdges if needed }
  InvalidateManifoldAndBorderEdges;
  inherited;
end;

function TShapeShadowVolumes.TrianglesListShadowCasters: TTrianglesShadowCastersList;

  function CreateTrianglesListShadowCasters: TTrianglesShadowCastersList;

    function ShadowCaster(AShape: TShape): boolean;
    var
      Shape: TAbstractShapeNode;
    begin
      Shape := AShape.State.ShapeNode;
      Result := not (
        (Shape <> nil) and
        (Shape.FdAppearance.Value <> nil) and
        (Shape.FdAppearance.Value is TAppearanceNode) and
        (not TAppearanceNode(Shape.FdAppearance.Value).FdShadowCaster.Value));
    end;

  var
    TriangleAdder: TTriangleAdder;
    Shape: TShape;
  begin
    Shape := TShape(FShape);

    Result := TTrianglesShadowCastersList.Create;
    try
      Result.Capacity := Shape.TrianglesCount;
      TriangleAdder := TTriangleAdder.Create;
      try
        TriangleAdder.TriangleList := Result;
        if ShadowCaster(Shape) then
          Shape.LocalTriangulate({$ifdef FPC}@{$endif}TriangleAdder.AddTriangle,
            { FrontFaceAlwaysCcw should not matter } false);
        if LogShadowVolumes then
          WritelnLog('Shadow volumes', Format('Shadows casters triangles: %d',
            [Result.Count]));
      finally FreeAndNil(TriangleAdder) end;
    except Result.Free; raise end;
  end;

begin
  if not (svTrianglesListShadowCasters in Validities) then
  begin
    FreeAndNil(FTrianglesListShadowCasters);
    FTrianglesListShadowCasters := CreateTrianglesListShadowCasters;
    Include(Validities, svTrianglesListShadowCasters);
  end;

  Result := FTrianglesListShadowCasters;
end;

procedure TShapeShadowVolumes.InvalidateTrianglesListShadowCasters;
begin
  Exclude(Validities, svTrianglesListShadowCasters);
  FreeAndNil(FTrianglesListShadowCasters);
end;

procedure TShapeShadowVolumes.CalculateIfNeededManifoldAndBorderEdges;

  { Sets FManifoldEdges and FBorderEdges. Assumes that FManifoldEdges and
    FBorderEdges are @nil on enter. }
  procedure CalculateManifoldAndBorderEdges;

    { If the counterpart of this edge (edge from neighbor) exists in
      FBorderEdges, then it adds this edge (along with it's counterpart)
      to FManifoldEdges.

      Otherwise, it just adds the edge to FBorderEdges. This can happen
      if it's the 1st time this edge occurs, or maybe the 3d one, 5th...
      all odd occurrences, assuming that ordering of faces is consistent,
      so that counterpart edges are properly detected. }
    procedure AddEdgeCheckManifold(
      const TriangleIndex: Cardinal;
      const V0: TVector3;
      const V1: TVector3;
      const VertexIndex: Cardinal;
      Triangles: TTriangle3List);
    var
      I: Integer;
      EdgePtr: PEdge;
    begin
      if FBorderEdges.Count <> 0 then
      begin
        EdgePtr := PEdge(FBorderEdges.L);
        for I := 0 to FBorderEdges.Count - 1 do
        begin
          { It would also be possible to get EdgePtr^.V0/1 by code like

            TrianglePtr := @Triangles.L[EdgePtr^.Triangles[0]];
            EdgeV0 := @TrianglePtr^[EdgePtr^.VertexIndex];
            EdgeV1 := @TrianglePtr^[(EdgePtr^.VertexIndex + 1) mod 3];

            But, see TEdge.V0/1 comments --- current version is
            a little faster.
          }

          { Triangles must be consistently ordered on a manifold,
            so the second time an edge is present, we know it must
            be in different order. So we compare V0 with EdgeV1
            (and V1 with EdgeV0), no need to compare V1 with EdgeV1. }
          if TVector3.PerfectlyEquals(V0, EdgePtr^.V1) and
             TVector3.PerfectlyEquals(V1, EdgePtr^.V0) then
          begin
            EdgePtr^.Triangles[1] := TriangleIndex;

            { Move edge to FManifoldEdges: it has 2 neighboring triangles now. }
            FManifoldEdges.Add^ := EdgePtr^;

            { Remove this from FBorderEdges.
              Note that we delete from FBorderEdges fast, using assignment and
              deleting only from the end (normal Delete would want to shift
              FBorderEdges contents in memory, to preserve order of items;
              but we don't care about order). }
            EdgePtr^ := FBorderEdges.L[FBorderEdges.Count - 1];
            FBorderEdges.Count := FBorderEdges.Count - 1;

            Exit;
          end;
          Inc(EdgePtr);
        end;
      end;

      { New edge: add new item to FBorderEdges }
      EdgePtr := PEdge(FBorderEdges.Add);
      EdgePtr^.VertexIndex := VertexIndex;
      EdgePtr^.Triangles[0] := TriangleIndex;
      EdgePtr^.V0 := V0;
      EdgePtr^.V1 := V1;
    end;

  var
    I: Integer;
    Triangles: TTriangle3List;
    TrianglePtr: PTriangle3;
  begin
    Assert(FManifoldEdges = nil);
    Assert(FBorderEdges = nil);

    { It's important here that TrianglesListShadowCasters guarantees that only
      valid triangles are included. Otherwise degenerate triangles could make
      shadow volumes rendering result bad. }
    Triangles := TrianglesListShadowCasters;

    FManifoldEdges := TEdgeList.Create;
    { There is a precise relation between number of edges and number of faces
      on a closed manifold: E = T * 3 / 2. }
    FManifoldEdges.Capacity := Triangles.Count * 3 div 2;

    { FBorderEdges are edges that have no neighbor,
      i.e. have only one adjacent triangle. At the end, what's left here
      will be simply left as BorderEdges. }
    FBorderEdges := TEdgeList.Create;
    FBorderEdges.Capacity := Triangles.Count * 3 div 2;

    TrianglePtr := PTriangle3(Triangles.L);
    for I := 0 to Triangles.Count - 1 do
    begin
      { TrianglePtr points to Triangles[I] now }
      AddEdgeCheckManifold(I, TrianglePtr^.Data[0], TrianglePtr^.Data[1], 0, Triangles);
      AddEdgeCheckManifold(I, TrianglePtr^.Data[1], TrianglePtr^.Data[2], 1, Triangles);
      AddEdgeCheckManifold(I, TrianglePtr^.Data[2], TrianglePtr^.Data[0], 2, Triangles);
      Inc(TrianglePtr);
    end;

    if LogShadowVolumes then
      WritelnLog('Shadow volumes', Format(
        'Edges: %d manifold, %d border',
        [FManifoldEdges.Count, FBorderEdges.Count] ));
  end;

begin
  if not (svManifoldAndBorderEdges in Validities) then
  begin
    CalculateManifoldAndBorderEdges;
    Include(Validities, svManifoldAndBorderEdges);
  end;
end;

function TShapeShadowVolumes.ManifoldEdges: TEdgeList;
begin
  CalculateIfNeededManifoldAndBorderEdges;
  Result := FManifoldEdges;
end;

function TShapeShadowVolumes.BorderEdges: TEdgeList;
begin
  CalculateIfNeededManifoldAndBorderEdges;
  Result := FBorderEdges;
end;

procedure TShapeShadowVolumes.InvalidateManifoldAndBorderEdges;
begin
  Exclude(Validities, svManifoldAndBorderEdges);
  FreeAndNil(FManifoldEdges);
  FreeAndNil(FBorderEdges);
end;

procedure TShapeShadowVolumes.PrepareResources;
begin
  TrianglesListShadowCasters;
  CalculateIfNeededManifoldAndBorderEdges;
end;

procedure TShapeShadowVolumes.FreeResources;
begin
  InvalidateTrianglesListShadowCasters;
  InvalidateManifoldAndBorderEdges;
end;

function CalculateDetectedWholeSceneManifold(const ShapeListCore: TObject): Boolean;
var
  ShapeList: TShapeList absolute ShapeListCore;
  Shape1, Shape2: TShape;
  BorderEdges1, BorderEdges2: TEdgeList;
  BorderEdge1, BorderEdge2: PEdge;
  BorderEdge1Index, BorderEdge2Index: Integer;
  Shape1V0, Shape1V1, Shape2V0, Shape2V1: TVector3;
  AnyBorderEdges: Boolean;
begin
  { Once ManifoldEdges and BorderEdges for all shapes are calculated,
    check is scene manifold "as a whole". }

  Result := true; // assume yes
  AnyBorderEdges := false;

  for Shape1 in ShapeList do
  begin
    BorderEdges1 := Shape1.InternalShadowVolumes.BorderEdges;
    BorderEdge1 := PEdge(BorderEdges1.L);
    AnyBorderEdges := AnyBorderEdges or (BorderEdges1.Count <> 0);
    for BorderEdge1Index := 0 to BorderEdges1.Count - 1 do
    begin
      // do not match, if this edge is already matched
      if BorderEdge1^.Triangles[1] = High(Cardinal) then
      begin
        Inc(BorderEdge1);
        Continue;
      end;

      Shape1V0 := Shape1.State.Transformation.Transform.MultPoint(BorderEdge1^.V0);
      Shape1V1 := Shape1.State.Transformation.Transform.MultPoint(BorderEdge1^.V1);

      for Shape2 in ShapeList do
      begin
        if Shape1 = Shape2 then Continue;

        BorderEdges2 := Shape2.InternalShadowVolumes.BorderEdges;
        BorderEdge2 := PEdge(BorderEdges2.L);
        for BorderEdge2Index := 0 to BorderEdges2.Count - 1 do
        begin
          // do not match, if this edge is already matched
          if BorderEdge2^.Triangles[1] = High(Cardinal) then
          begin
            Inc(BorderEdge2);
            Continue;
          end;

          Shape2V0 := Shape2.State.Transformation.Transform.MultPoint(BorderEdge2^.V0);
          Shape2V1 := Shape2.State.Transformation.Transform.MultPoint(BorderEdge2^.V1);

          { Try both orders: two border edges can match in any order,
            our border edges rendering tolerates it.

            Compare with epsilon or not?
            Better not. Comparing with epsilon may be faulty
            (if OpenGL calculations lead to slightly different results,
            we will have rendering artifacts).
            And it seems comparing precisely actually works in our testcases,
            it detects scenes as 2-manifold correctly. }
          // if ( TVector3.Equals(Shape1V0, Shape2V1{, Epsilon}) and
          //      TVector3.Equals(Shape1V1, Shape2V0{, Epsilon}) ) or
          //    ( TVector3.Equals(Shape1V0, Shape2V0{, Epsilon}) and
          //      TVector3.Equals(Shape1V1, Shape2V1{, Epsilon}) ) then
          if ( TVector3.PerfectlyEquals(Shape1V0, Shape2V1) and
               TVector3.PerfectlyEquals(Shape1V1, Shape2V0) ) or
             ( TVector3.PerfectlyEquals(Shape1V0, Shape2V0) and
               TVector3.PerfectlyEquals(Shape1V1, Shape2V1) ) then
          begin
            // mark that this edge is already matched
            BorderEdge2^.Triangles[1] := High(Cardinal);
            BorderEdge1^.Triangles[1] := High(Cardinal);
            Break;
          end;
          Inc(BorderEdge2);
        end;

        if BorderEdge1^.Triangles[1] = High(Cardinal) then
          Break; // found match for BorderEdge1
      end;

      // not found any match for BorderEdge1, in all shapes
      if BorderEdge1^.Triangles[1] <> High(Cardinal) then
      begin
        Result := false;
        {.$define CalculateDetectedWholeSceneManifold_Log}
        {$ifdef CalculateDetectedWholeSceneManifold_Log}
        WritelnLog('Scene %s %s detected as NOT 2-manifold.', [Name, UriDisplay(Url)]);
        {$endif}

        { Do not Exit to keep detecting, and match as much edges as possible,
          even though we know Result is false. }
        if not WholeSceneManifoldDetectionForceAllEdges then
          Exit;
      end;

      Inc(BorderEdge1);
    end;
  end;

  { When the scene is trivially detected as "whole scene is 2-manifold"
    just because there are no border edges, then actually each shape is
    already a 2-manifold. We don't need in this case the EffectiveWholeSceneManifold
    algorithm, better to cull and render shadow volumes per shape. }
  if not AnyBorderEdges then
  begin
    {$ifdef CalculateDetectedWholeSceneManifold_Log}
    WritelnLog('Scene %s %s detected as 2-manifold, but trivially, because there are no border edges. Disabling EffectiveWholeSceneManifold.', [Name, UriDisplay(Url)]);
    {$endif}
    Result := false;
  end else
  begin
    {$ifdef CalculateDetectedWholeSceneManifold_Log}
    WritelnLog('Scene %s %s detected as 2-manifold.', [Name, UriDisplay(Url)]);
    {$endif}
  end;
end;

end.
