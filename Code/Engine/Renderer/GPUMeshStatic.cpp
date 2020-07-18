#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Math/Primatives/LineSeg3D.hpp"
#include "Engine/Core/VertexTypes/Vertex_PCUTBN.hpp"
#include "Engine/IO/ObjFileUtils.hpp"


#include "GPUMesh.hpp"

static Rgba8 g_Color( 255, 255, 255 );
static VertexMaster g_CubeVertexes[] = {
    // Front Face - 0, 1, 2, 3
    VertexMaster( Vec3( 1.f, 1.f, -1.f ), g_Color, Vec2( 0.f, 0.f ), Vec3( 1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, -1 ) ),   // Top Left
    VertexMaster( Vec3( 1.f, -1.f, -1.f ), g_Color, Vec2( 0.f, 1.f ), Vec3( 1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, -1 ) ),  // Bottom Left
    VertexMaster( Vec3( -1.f, -1.f, -1.f ), g_Color, Vec2( 1.f, 1.f ), Vec3( 1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, -1 ) ),   // Bottom Right
    VertexMaster( Vec3( -1.f, 1.f, -1.f ), g_Color, Vec2( 1.f, 0.f ), Vec3( 1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, -1 ) ),    // Top Right

    // Right Face - 4, 5, 6, 7
    VertexMaster( Vec3( -1.f, 1.f, -1.f ), g_Color, Vec2( 0.f, 0.f ), Vec3( 0, 0, -1 ),
                  Vec3( 0, 1, 0 ), Vec3( -1, 0, 0 ) ),   // Top Right
    VertexMaster( Vec3( -1.f, -1.f, -1.f ), g_Color, Vec2( 0.f, 1.f ), Vec3( 0, 0, -1 ),
                  Vec3( 0, 1, 0 ), Vec3( -1, 0, 0 ) ),  // Bottom Right
    VertexMaster( Vec3( -1.f, -1.f, 1.f ), g_Color, Vec2( 1.f, 1.f ), Vec3( 0, 0, -1 ),
                  Vec3( 0, 1, 0 ), Vec3( -1, 0, 0 ) ),   // Bottom Left
    VertexMaster( Vec3( -1.f, 1.f, 1.f ), g_Color, Vec2( 1.f, 0.f ), Vec3( 0, 0, -1 ),
                  Vec3( 0, 1, 0 ), Vec3( -1, 0, 0 ) ),    // Top Left

    // Back Face - 8, 9, 10, 11
    VertexMaster( Vec3( -1.f, 1.f, 1.f ), g_Color, Vec2( 0.f, 0.f ), Vec3( -1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, 1 ) ),     // Top Left
    VertexMaster( Vec3( -1.f, -1.f, 1.f ), g_Color, Vec2( 0.f, 1.f ), Vec3( -1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, 1 ) ),    // Bottom Left
    VertexMaster( Vec3( 1.f, -1.f, 1.f ), g_Color, Vec2( 1.f, 1.f ), Vec3( -1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, 1 ) ),   // Bottom Right
    VertexMaster( Vec3( 1.f, 1.f, 1.f ), g_Color, Vec2( 1.f, 0.f ), Vec3( -1, 0, 0 ),
                  Vec3( 0, 1, 0 ), Vec3( 0, 0, 1 ) ),    // Top Right

    // Left Face - 12, 13, 14, 15
    VertexMaster( Vec3( 1.f, 1.f, 1.f ), g_Color, Vec2( 0.f, 0.f ), Vec3( 0, 0, 1 ),
                  Vec3( 0, 1, 0 ), Vec3( 1, 0, 0 ) ),     // Top Right
    VertexMaster( Vec3( 1.f, -1.f, 1.f ), g_Color, Vec2( 0.f, 1.f ), Vec3( 0, 0, 1 ),
                  Vec3( 0, 1, 0 ), Vec3( 1, 0, 0 ) ),    // Bottom Right
    VertexMaster( Vec3( 1.f, -1.f, -1.f ), g_Color, Vec2( 1.f, 1.f ), Vec3( 0, 0, 1 ),
                  Vec3( 0, 1, 0 ), Vec3( 1, 0, 0 ) ),   // Bottom Left
    VertexMaster( Vec3( 1.f, 1.f, -1.f ), g_Color, Vec2( 1.f, 0.f ), Vec3( 0, 0, 1 ),
                  Vec3( 0, 1, 0 ), Vec3( 1, 0, 0 ) ),    // Top Left

    // Top Face - 16, 17, 18, 19
    VertexMaster( Vec3( 1.f, 1.f, 1.f ), g_Color, Vec2( 0.f, 1.f ), Vec3(-1, 0, 0), Vec3(0,0,1), Vec3(0,1,0) ),    // Top Left
    VertexMaster( Vec3( 1.f, 1.f, -1.f ), g_Color, Vec2( 1.f, 1.f ), Vec3( -1, 0, 0 ), Vec3( 0,0,1 ), Vec3( 0,1,0 ) ),   // Bottom Left
    VertexMaster( Vec3( -1.f, 1.f, -1.f ), g_Color, Vec2( 1.f, 0.f ), Vec3( -1, 0, 0 ), Vec3( 0,0,1 ), Vec3( 0,1,0 ) ),    // Bottom Right
    VertexMaster( Vec3( -1.f, 1.f, 1.f ), g_Color, Vec2( 0.f, 0.f ), Vec3( -1, 0, 0 ), Vec3( 0,0,1 ), Vec3( 0,1,0 ) ),     // Top Right

    // Bottom Face- 20, 21, 22, 23
    VertexMaster( Vec3( 1.f, -1.f, -1.f ), g_Color, Vec2( 1.f, 0.f ), Vec3( -1, 0, 0 ), Vec3( 0,0,-1 ), Vec3( 0,-1,0 ) ),  // Top Left
    VertexMaster( Vec3( 1.f, -1.f, 1.f ), g_Color, Vec2( 0.f, 0.f ), Vec3( -1, 0, 0 ), Vec3( 0,0,-1 ), Vec3( 0,-1,0 ) ),   // Bottom Left
    VertexMaster( Vec3( -1.f, -1.f, 1.f ), g_Color, Vec2( 0.f, 1.f ), Vec3( -1, 0, 0 ), Vec3( 0,0,-1 ), Vec3( 0,-1,0 ) ),    // Bottom Right
    VertexMaster( Vec3( -1.f, -1.f, -1.f ), g_Color, Vec2( 1.f, 1.f ), Vec3( -1, 0, 0 ), Vec3( 0,0,-1 ), Vec3( 0,-1,0 ) ),   // Top Right

};

static unsigned int g_CubeIndexes[] = {
    2, 3, 0,
    1, 2, 0,

    6, 7, 4,
    5, 6, 4,

    10, 11, 8,
    9, 10, 8,

    14, 15, 12,
    13, 14, 12,

    18, 19, 16,
    17, 18, 16,

    22, 23, 20,
    21, 22, 20,
};

static float SurfaceFunctionZero( const Vec2& uv )
{
    UNUSED( uv );
    return 0.f;
}

static void MakeSurface( std::vector<VertexMaster>& planeVertexes,
                         std::vector<unsigned int>& planeIndexes,
                         const Vec2& halfSize, const Vec2& splits,
                         const Rgba8& color,
                         SurfaceFunction* surfaceFunction = nullptr )
{
    if ( surfaceFunction == nullptr )
    {
        surfaceFunction = &SurfaceFunctionZero;
    }

    const Vec2 dimensions = halfSize * 2.f;

    const int horizontalSize = static_cast<int>(splits.y) + 2;
    const float xDist = (splits.y == 0)
                            ? dimensions.x
                            : dimensions.x / static_cast<float>(splits.y + 1);
    const float yDist = (splits.x == 0)
                            ? dimensions.y
                            : dimensions.y / static_cast<float>(splits.x + 1);
    const float xUv = (splits.y == 0) ? 1.f : 1.f / static_cast<float>(splits.y + 1);
    const float yUv = (splits.x == 0) ? 1.f : 1.f / static_cast<float>(splits.x + 1);

    size_t ySteps = 0;

    Vec3 topLeft( -halfSize.x, halfSize.y, 0.f );
    Vec2 topLeftUv = Vec2::ZERO;
    do
    {
        size_t xSteps = 0;
        topLeftUv.x = 0;
        topLeft.x = -halfSize.x;
        topLeft.z = surfaceFunction( topLeftUv );
        planeVertexes.emplace_back( topLeft, color, topLeftUv, Vec3( 1, 0, 0 ), Vec3( 0, 1, 0 ),
                                    Vec3( 0, 0, 1 ) );
        do
        {
            topLeftUv.x += xUv;
            topLeft.x += xDist;
            topLeft.z = surfaceFunction( topLeftUv );
            planeVertexes.emplace_back( topLeft, color, topLeftUv, Vec3( 1, 0, 0 ), Vec3( 0, 1, 0 ),
                                        Vec3( 0, 0, 1 ) );

            xSteps++;
            // first entry xSteps = 1, ySteps = 1;
            if ( ySteps > 0 )
            {
                const int topLeftIndex = horizontalSize * static_cast<int>(ySteps - 1) +
                    static_cast<int>(xSteps - 1);
                const int bottomLeftIndex = horizontalSize * static_cast<int>(ySteps) +
                    static_cast<int>(xSteps - 1);

                planeIndexes.push_back( topLeftIndex );
                planeIndexes.push_back( bottomLeftIndex );
                planeIndexes.push_back( topLeftIndex + 1 );

                planeIndexes.push_back( bottomLeftIndex );
                planeIndexes.push_back( bottomLeftIndex + 1 );
                planeIndexes.push_back( topLeftIndex + 1 );
            }
        }
        while ( xSteps <= splits.y );

        topLeft.y -= yDist;
        topLeftUv.y += yUv;
        ySteps++;
    }
    while ( ySteps <= splits.x + 1 );
}

static void MakeSphereUv( std::vector<VertexMaster>& planeVertexes,
                          std::vector<unsigned int>& planeIndexes,
                          const float radius, const Vec2& lines,
                          const Rgba8& color )
{
    float theta = 0.f; // Angle from the positive y to the xz plane

    // const int horizontalSize = static_cast<int>(lines.x) + 2;
    const int verticalLines = static_cast<int>(lines.x + 1);
    const int horizontalLines = static_cast<int>(lines.y) + 1;
    const float phiDelta = 360.f / static_cast<float>(lines.x);
    const float thetaDelta = 180.f / static_cast<float>(horizontalLines);

    Vec3 point = Vec3::ZERO;
    Vec2 pointUv = Vec2::ZERO;
    size_t ySteps = 0;
    do
    {
        size_t xSteps = 0;
        float phi = 0.f; // Angle from the positive x to the zy plane

        const float sinTheta = sinfDegrees( theta );
        const float cosTheta = cosfDegrees( theta );

        point.y = radius * cosTheta;
        do
        {
            const float sinPhi = sinfDegrees( phi );
            const float cosPhi = cosfDegrees( phi );

            pointUv.x = RangeMapFloat( 0.f, 360.f, 1.f, 0.f, phi );
            pointUv.y = RangeMapFloat( 0.f, 180.f, 0.f, 1.f, theta );
            point.x = radius * sinTheta * cosPhi;
            point.z = radius * sinTheta * sinPhi;

            const Vec3 tangent( -sinPhi, 0.f, cosPhi );
            const Vec3 normal = point.GetNormalized();
            const Vec3 bitangent = normal.GetCross( tangent );
            planeVertexes.emplace_back( point, color, pointUv, tangent, bitangent, normal );

            // first entry xSteps = 1, ySteps = 1;
            if ( xSteps < verticalLines - 1 && ySteps < horizontalLines )
            {
                const int topLeftIndex = verticalLines * static_cast<int>(ySteps + 1) +
                    static_cast<int>(xSteps);
                const int bottomRightIndex = verticalLines * static_cast<int>(ySteps) +
                    static_cast<int>(xSteps + 1);

                planeIndexes.push_back( bottomRightIndex - 1 );
                planeIndexes.push_back( bottomRightIndex );
                planeIndexes.push_back( topLeftIndex + 1 );

                planeIndexes.push_back( topLeftIndex );
                planeIndexes.push_back( bottomRightIndex - 1 );
                planeIndexes.push_back( topLeftIndex + 1 );
            }

            phi += phiDelta;
            xSteps++;
        }
        while ( xSteps < verticalLines );

        // pointUv.y += yUv;
        theta += thetaDelta;
        ySteps++;
    }
    while ( ySteps <= horizontalLines );
}

STATIC GPUMesh* GPUMesh::CreateFromFile( RenderContext* ctx, const std::string& fileName, 
                                         const MeshLoadOptions& meshLoadOptions )
{
    GPUMesh* mesh = new GPUMesh( ctx );
    std::vector<VertexMaster> vertexMaster;
    std::vector<unsigned int> indexes;

    LoadFromObjFile( fileName, vertexMaster, indexes, meshLoadOptions );

    std::vector<Vertex_PCUTBN> vertexPCUTBN;
    Vertex_PCUTBN::ConvertFromMaster( vertexPCUTBN, vertexMaster );
    mesh->UpdateVertexes( vertexPCUTBN );
    mesh->UpdateIndexes( indexes );

    return mesh;
}


STATIC GPUMesh* GPUMesh::CreateCube( RenderContext* ctx, const Vec3& halfSize, const Rgba8& tint )
{
    GPUMesh* cube = new GPUMesh( ctx );
    std::vector<VertexMaster> cubeVertexes;

    for ( VertexMaster transform : g_CubeVertexes )
    {
        transform.position *= halfSize;
        transform.color = tint;
        cubeVertexes.push_back( transform );
    }

    TransformVertexArray( cubeVertexes, Engine::GetCanonicalTransform() );

    std::vector<Vertex_PCUTBN> pcutbnVertexes;
    Vertex_PCUTBN::ConvertFromMaster( pcutbnVertexes, cubeVertexes );

    cube->UpdateVertexes( pcutbnVertexes );
    cube->UpdateIndexes( g_CubeIndexes, 36 );

    return cube;
}

STATIC GPUMesh* GPUMesh::CreateLine( RenderContext* ctx, const LineSeg3D& lineSeg,
                                     const float thickness,
                                     const Rgba8& tint )
{
    GPUMesh* line = new GPUMesh( ctx );

    std::vector<VertexMaster> lineVertexes;
    std::vector<unsigned int> lineIndexes;

    const Vec3 displacement = lineSeg.end - lineSeg.start;
    const float lineRadius = thickness * .5f;

    Vec3 forward = displacement.GetNormalized();
    Vec3 left = forward.GetLeft();
    Vec3 top = forward.GetCross( left );

    forward *= lineRadius;
    left *= lineRadius;
    top *= lineRadius;

    // "Horizontal plane"
    lineVertexes.emplace_back( lineSeg.start - forward - left, tint );
    lineVertexes.emplace_back( lineSeg.end + forward - left, tint );
    lineVertexes.emplace_back( lineSeg.end + forward + left, tint );
    lineVertexes.emplace_back( lineSeg.start - forward + left, tint );

    lineIndexes.push_back( 0 );
    lineIndexes.push_back( 1 );
    lineIndexes.push_back( 2 );
    lineIndexes.push_back( 0 );
    lineIndexes.push_back( 2 );
    lineIndexes.push_back( 3 );

    lineIndexes.push_back( 0 );
    lineIndexes.push_back( 2 );
    lineIndexes.push_back( 1 );
    lineIndexes.push_back( 0 );
    lineIndexes.push_back( 3 );
    lineIndexes.push_back( 2 );


    // "Vertical plane"
    lineVertexes.emplace_back( lineSeg.start - forward - top, tint );
    lineVertexes.emplace_back( lineSeg.end + forward - top, tint );
    lineVertexes.emplace_back( lineSeg.end + forward + top, tint );
    lineVertexes.emplace_back( lineSeg.start - forward + top, tint );

    lineIndexes.push_back( 4 );
    lineIndexes.push_back( 5 );
    lineIndexes.push_back( 6 );
    lineIndexes.push_back( 4 );
    lineIndexes.push_back( 6 );
    lineIndexes.push_back( 7 );

    lineIndexes.push_back( 4 );
    lineIndexes.push_back( 6 );
    lineIndexes.push_back( 5 );
    lineIndexes.push_back( 4 );
    lineIndexes.push_back( 7 );
    lineIndexes.push_back( 6 );

    std::vector<Vertex_PCUTBN> templatedType;
    Vertex_PCUTBN::ConvertFromMaster( templatedType, lineVertexes );
    line->UpdateVertexes( templatedType );
    line->UpdateIndexes( lineIndexes );

    return line;
}

STATIC GPUMesh* GPUMesh::CreatePlane( RenderContext* ctx, const Vec2& halfSize,
                                      const Vec2& splits, const Rgba8& tint )
{
    GPUMesh* plane = new GPUMesh( ctx );

    std::vector<VertexMaster> planeVertexes;
    std::vector<unsigned int> planeIndexes;

    MakeSurface( planeVertexes, planeIndexes, halfSize, splits, tint );

    std::vector<Vertex_PCUTBN> templatedType;
    Vertex_PCUTBN::ConvertFromMaster( templatedType, planeVertexes );
    plane->UpdateVertexes( templatedType );
    plane->UpdateIndexes( planeIndexes );

    return plane;
}

STATIC GPUMesh* GPUMesh::CreatePlane( RenderContext* ctx,
                                      const Vec2& halfSize,
                                      SurfaceFunction* heightFunction,
                                      const Vec2& splits, const Rgba8& tint )
{
    GPUMesh* plane = new GPUMesh( ctx );

    std::vector<VertexMaster> planeVertexes;
    std::vector<unsigned int> planeIndexes;

    MakeSurface( planeVertexes, planeIndexes, halfSize, splits, tint, heightFunction );

    std::vector<Vertex_PCUTBN> templatedType;
    Vertex_PCUTBN::ConvertFromMaster( templatedType, planeVertexes );
    plane->UpdateVertexes( templatedType );
    plane->UpdateIndexes( planeIndexes );

    return plane;
}

STATIC GPUMesh* GPUMesh::CreateSphereUv( RenderContext* ctx, const float radius, const Vec2& splits,
                                         const Rgba8& tint )
{
    GPUMesh* sphere = new GPUMesh( ctx );

    std::vector<VertexMaster> sphereVertexes;
    std::vector<unsigned int> sphereIndexes;

    MakeSphereUv( sphereVertexes, sphereIndexes, radius, splits, tint );

    std::vector<Vertex_PCUTBN> templatedType;
    Vertex_PCUTBN::ConvertFromMaster( templatedType, sphereVertexes );
    sphere->UpdateVertexes( templatedType );
    sphere->UpdateIndexes( sphereIndexes );

    return sphere;
}
