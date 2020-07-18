#include "MeshUtils.hpp"

#include "Engine/Console/Console.hpp"


#include "ThirdParty/MikkTSpace/mikktspace.h"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Math/Primatives/AABB3.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"
#include "Engine/Core/Math/Primatives/LineSeg3D.hpp"
#include "Engine/Core/Math/Primatives/Mat44.hpp"
#include "Engine/Core/Math/Primatives/OBB2.hpp"

static constexpr int CIRCLE_RADIUSES = 64;

static constexpr float ARROW_RATIO_TO_LINE = .3f;
static constexpr float ARROW_THICKNESS_TO_LENGTH = .8f;

static int GetNumFaces( const SMikkTSpaceContext* context)
{
    MikkTangentSupport& helper = *reinterpret_cast<MikkTangentSupport*>(context->m_pUserData);

    if( helper.useIndex )
    {
        return static_cast<int>(helper.indexes->size()) / 3;
    }
    else
    {
        return static_cast<int>(helper.vertexes->size()) / 3;
    }
}

static int GetNumVerticesOfFace( const SMikkTSpaceContext* context, const int face)
{
    UNUSED( context );
    UNUSED( face );

    return 3;
}

static void GetPosition( const SMikkTSpaceContext* context, float posOut[], const int face, const int vert )
{
    MikkTangentSupport& helper = *reinterpret_cast<MikkTangentSupport*>(context->m_pUserData);
    const int index = face * 3 + vert;

    Vec3 position;
    if( helper.useIndex )
    {
        
        position = helper.vertexes->at( helper.indexes->at( index ) ).position;
    }
    else
    {
        position = helper.vertexes->at( index ).position;
    }

    posOut[ 0 ] = position.x;
    posOut[ 1 ] = position.y;
    posOut[ 2 ] = position.z;
}

static void GetNormal( const SMikkTSpaceContext* context, float norOut[], const int face, const int vert )
{
    MikkTangentSupport& helper = *reinterpret_cast<MikkTangentSupport*>(context->m_pUserData);
    const int index = face * 3 + vert;

    Vec3 normal;
    if ( helper.useIndex )
    {

        normal = helper.vertexes->at( helper.indexes->at( index ) ).normal;
    }
    else
    {
        normal = helper.vertexes->at( index ).normal;
    }

    norOut[ 0 ] = normal.x;
    norOut[ 1 ] = normal.y;
    norOut[ 2 ] = normal.z;
}

static void GetTexCoord( const SMikkTSpaceContext* context, float texCoordOut[], const int face, const int vert )
{
    MikkTangentSupport& helper = *reinterpret_cast<MikkTangentSupport*>(context->m_pUserData);
    const int index = face * 3 + vert;

    Vec2 texCoord;
    if ( helper.useIndex )
    {

        texCoord = helper.vertexes->at( helper.indexes->at( index ) ).uv;
    }
    else
    {
        texCoord = helper.vertexes->at( index ).uv;
    }

    texCoordOut[ 0 ] = texCoord.x;
    texCoordOut[ 1 ] = texCoord.y;
}

static void SetTangentSpaceBasic( const SMikkTSpaceContext* context, const float tangentCalc[], const float sign, const int face, const int vert)
{
    MikkTangentSupport& helper = *reinterpret_cast<MikkTangentSupport*>(context->m_pUserData);
    const int index = face * 3 + vert;

    VertexMaster* vertex = nullptr;
    if ( helper.useIndex )
    {
        vertex = &helper.vertexes->at( helper.indexes->at( index ) );
    }
    else
    {
        vertex = &helper.vertexes->at( index );
    }

    vertex->tangent = Vec3( tangentCalc[ 0 ], tangentCalc[ 1 ], tangentCalc[ 2 ] );
    vertex->bitangent = sign * vertex->normal.GetCross( vertex->tangent ).GetNormalized();
}

void MikkTangentCalculation( MikkTangentSupport* support )
{
    SMikkTSpaceInterface interface;
    memset( &interface, 0, sizeof( interface ) );
    interface.m_getNumFaces = GetNumFaces;
    interface.m_getNumVerticesOfFace = GetNumVerticesOfFace;
    interface.m_getPosition = GetPosition;
    interface.m_getNormal = GetNormal;
    interface.m_getTexCoord = GetTexCoord;
    interface.m_setTSpaceBasic = SetTangentSpaceBasic;

    SMikkTSpaceContext context;
    context.m_pInterface = &interface;
    context.m_pUserData = support;

    genTangSpaceDefault( &context );
}

void MikkTangentCalculation( std::vector<VertexMaster>& vertexes )
{
    MikkTangentSupport support;
    support.vertexes = &vertexes;
    support.useIndex = false;

    MikkTangentCalculation( &support );
}

void MikkTangentCalculation( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes )
{
    MikkTangentSupport support;
    support.vertexes = &vertexes;
    support.indexes = &indexes;
    support.useIndex = true;

    MikkTangentCalculation( &support );
}

// Purposefully overflow lastIndex 
static bool IsVertexInVector( const VertexMaster& value, const std::vector<VertexMaster>& vector, 
                              unsigned int& foundIndex, const size_t lastIndex = -1 )
{
    const size_t size = Minu( vector.size(), lastIndex );

    for( size_t index = 0; index < size; ++index )
    {
        if( value.IsMostlyEqual( vector.at( index ) ) )
        {
            foundIndex = static_cast<unsigned int>(index);
            return true;
        }
    }

    return false;
}

void CleanMesh( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes )
{
    unsigned int insertIndex = 0;
    size_t searchIndex = 0;

    for( const VertexMaster& vertex : vertexes )
    {
        unsigned int foundIndex;
        const bool foundInVector = IsVertexInVector( vertex, vertexes, foundIndex, searchIndex );
        if( foundInVector )
        {
            indexes.push_back( foundIndex );
        }
        else
        {
            vertexes.at( insertIndex ) = vertex;
            indexes.push_back( insertIndex );
            insertIndex++;
        }

        searchIndex++;
    }

    const size_t oldSize = vertexes.size();
    const size_t newSize = insertIndex;
    const size_t oldByteSize = oldSize * sizeof( VertexMaster );
    const size_t newByteSize = newSize * sizeof( VertexMaster );

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log(LOG_LOG, Stringf("Cleaned the mesh. %u(%u bytes) -> %u(%u bytes)", oldSize, oldByteSize, newSize, newByteSize));
#endif // !defined(ENGINE_DISABLE_CONSOLE)

    vertexes.resize( insertIndex );
}

void IndexMesh( const std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes )
{
    indexes.reserve( vertexes.size() );
    for( unsigned int index = 0; index < vertexes.size(); ++index )
    {
        indexes.push_back( index );
    }
}

void AppendLine( std::vector<VertexMaster>& vertexes, const LineSeg2D& lineSeg,
                 const Rgba8& startTint, const Rgba8& endTint, const float thickness )
{
    const Vec2 displacement = lineSeg.GetDisplacement();
    const float lineRadius = thickness * .5f;

    const Vec2 forward = lineRadius * displacement.GetNormalized();
    const Vec2 left = forward.GetRotated90Degrees();

    vertexes.emplace_back( lineSeg.start - forward - left, startTint );
    vertexes.emplace_back( lineSeg.end + forward - left, endTint );
    vertexes.emplace_back( lineSeg.end + forward + left, endTint );

    vertexes.emplace_back( lineSeg.end + forward + left, endTint );
    vertexes.emplace_back( lineSeg.start - forward + left, startTint );
    vertexes.emplace_back( lineSeg.start - forward - left, startTint );
}

void AppendLine( std::vector<VertexMaster>& vertexes, const LineSeg2D& lineSeg, const Rgba8& tint,
                 float thickness )
{
    AppendLine( vertexes, lineSeg, tint, tint, thickness );
}

void AppendRay( std::vector<VertexMaster>& vertexes,
                const LineSeg3D& lineSeg, const Rgba8& tint, float thickness )
{
    AppendRay( vertexes, lineSeg, tint, tint, thickness );
}

void AppendRay( std::vector<VertexMaster>& vertexes, const LineSeg3D& lineSeg, const Rgba8& startTint,
                const Rgba8& endTint, float thickness )
{
    const Vec3 displacement = lineSeg.end - lineSeg.start;
    const float lineRadius = thickness * .5f;

    Vec3 forward = displacement.GetNormalized();
    Vec3 left = forward.GetLeft();
    Vec3 top = forward.GetCross( left );

    forward *= lineRadius;
    left *= lineRadius;
    top *= lineRadius;

    // "Horizontal plane"
    vertexes.emplace_back( lineSeg.start - forward - left, startTint );
    vertexes.emplace_back( lineSeg.end + forward - left, endTint );
    vertexes.emplace_back( lineSeg.end + forward + left, endTint );
    vertexes.emplace_back( lineSeg.end + forward + left, endTint );
    vertexes.emplace_back( lineSeg.start - forward + left, startTint );
    vertexes.emplace_back( lineSeg.start - forward - left, startTint );

    vertexes.emplace_back( lineSeg.end + forward + left, endTint );
    vertexes.emplace_back( lineSeg.end + forward - left, endTint );
    vertexes.emplace_back( lineSeg.start - forward - left, startTint );
    vertexes.emplace_back( lineSeg.start - forward - left, startTint );
    vertexes.emplace_back( lineSeg.start - forward + left, startTint );
    vertexes.emplace_back( lineSeg.end + forward + left, endTint );

    // "Vertical plane"
    vertexes.emplace_back( lineSeg.start - forward - top, startTint );
    vertexes.emplace_back( lineSeg.end + forward - top, endTint );
    vertexes.emplace_back( lineSeg.end + forward + top, endTint );
    vertexes.emplace_back( lineSeg.end + forward + top, endTint );
    vertexes.emplace_back( lineSeg.start - forward + top, startTint );
    vertexes.emplace_back( lineSeg.start - forward - top, startTint );

    vertexes.emplace_back( lineSeg.end + forward + top, endTint );
    vertexes.emplace_back( lineSeg.end + forward - top, endTint );
    vertexes.emplace_back( lineSeg.start - forward - top, startTint );
    vertexes.emplace_back( lineSeg.start - forward - top, startTint );
    vertexes.emplace_back( lineSeg.start - forward + top, startTint );
    vertexes.emplace_back( lineSeg.end + forward + top, endTint );
}

void AppendRay( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes, const LineSeg3D& lineSeg, 
    const Rgba8& tint, float thickness )
{
    const Vec3 displacement = lineSeg.end - lineSeg.start;
    const float lineRadius = thickness * .5f;

    Vec3 forward = displacement.GetNormalized();
    Vec3 left = forward.GetLeft();
    Vec3 top = forward.GetCross( left );

    forward *= lineRadius;
    left *= lineRadius;
    top *= lineRadius;

    const unsigned int vertexCount = static_cast<unsigned int>(vertexes.size());
    // "Horizontal plane"
    vertexes.emplace_back( lineSeg.start - left, tint );
    vertexes.emplace_back( lineSeg.start + left, tint );
    vertexes.emplace_back( lineSeg.end + left, tint );
    vertexes.emplace_back( lineSeg.end - left, tint );

    // "Vertical plane"
    vertexes.emplace_back( lineSeg.start - top, tint );
    vertexes.emplace_back( lineSeg.end - top, tint );
    vertexes.emplace_back( lineSeg.end + top, tint );
    vertexes.emplace_back( lineSeg.start + top, tint );

    // "Horizontal plane front face"
    indexes.push_back( vertexCount );
    indexes.push_back( vertexCount + 1 );
    indexes.push_back( vertexCount + 2 );
    indexes.push_back( vertexCount );
    indexes.push_back( vertexCount + 2 );
    indexes.push_back( vertexCount + 3 );

    // "Horizontal plane back face"
    indexes.push_back( vertexCount );
    indexes.push_back( vertexCount + 2 );
    indexes.push_back( vertexCount + 1 );
    indexes.push_back( vertexCount );
    indexes.push_back( vertexCount + 3 );
    indexes.push_back( vertexCount + 2 );

    // "Vertical plane front face"
    indexes.push_back( vertexCount + 4 );
    indexes.push_back( vertexCount + 5 );
    indexes.push_back( vertexCount + 6 );
    indexes.push_back( vertexCount + 4 );
    indexes.push_back( vertexCount + 6 );
    indexes.push_back( vertexCount + 7 );

    // "Vertical plane back face"
    indexes.push_back( vertexCount + 4 );
    indexes.push_back( vertexCount + 6 );
    indexes.push_back( vertexCount + 5 );
    indexes.push_back( vertexCount + 4 );
    indexes.push_back( vertexCount + 7 );
    indexes.push_back( vertexCount + 6 );
}

//-----------------------------------------------------------------------------
// void AppendArrow( std::vector<Vertex_PCU>& vertexes, const Vec2& start, const Vec2& end,
//                   const Rgba8& color, float thickness )
// {
void AppendArrow( std::vector<VertexMaster>& vertexes, const LineSeg2D& lineSeg,
                  const Rgba8& startTint, const Rgba8& endTint, const float thickness )
{
    const float totalLength = lineSeg.GetLength();
    const float arrowPercent = Clamp( thickness * ARROW_RATIO_TO_LINE, .075f, .4f );
    const float arrowLength = totalLength * arrowPercent;
    const float arrowThickness = thickness * (1.f + ARROW_THICKNESS_TO_LENGTH);

    const Vec2 forward = lineSeg.GetDirection();
    const Vec2 left = forward.GetRotated90Degrees() * arrowThickness;
    const Vec2 arrowSplit = lineSeg.end - forward * arrowLength;
    const Rgba8 colorAtSplit = Rgba8::LerpAsHSL( startTint, endTint,
                                            (totalLength - arrowLength) / totalLength );

    AppendLine( vertexes, lineSeg, startTint, colorAtSplit, thickness );

    vertexes.emplace_back( arrowSplit + left, colorAtSplit );
    vertexes.emplace_back( arrowSplit - left, colorAtSplit );
    vertexes.emplace_back( lineSeg.end, endTint );
}

void AppendArrow3D( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes,
                    const LineSeg3D& lineSeg, const Rgba8& startTint, const Rgba8& endTint,
                    const float thickness )
{
    const float totalLength = lineSeg.GetLength();
    const float arrowPercent = Clamp( thickness * ARROW_RATIO_TO_LINE, .075f, .4f );
    const float arrowLength = totalLength * arrowPercent;
    const float arrowThickness = thickness * (1.f + ARROW_THICKNESS_TO_LENGTH);

    const Vec3 forward = lineSeg.GetDirection();
    const Vec3 arrowSplit = lineSeg.end - forward * arrowLength;
    const Rgba8 colorAtSplit = Rgba8::Lerp( startTint, endTint,
                                            (totalLength - arrowLength) / totalLength );

    AppendCylinder( vertexes, indexes, LineSeg3D( lineSeg.start, arrowSplit ), startTint,
                    colorAtSplit, thickness, thickness );
    AppendCone( vertexes, indexes, LineSeg3D( arrowSplit, lineSeg.end ), colorAtSplit, endTint,
                    arrowThickness );
}

void AppendArrow3D( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes,
                    const LineSeg3D& lineSeg, const Rgba8& tint, const float thickness )
{
    AppendArrow3D( vertexes, indexes, lineSeg, tint, tint, thickness );
}

//-----------------------------------------------------------------------------
void AppendCross( std::vector<VertexMaster>& vertexes,
                  const Vec2& center,
                  const float height,
                  const float thickness,
                  const Rgba8& color,
                  const float angleDegrees,
                  const float aspect )
{
    const OBB2 crossBounds = OBB2( center, Vec2( height * aspect, height ), angleDegrees );
    Vec2 cornerPoints[ 4 ];
    crossBounds.GetCornerPoints( cornerPoints );

    AppendLine( vertexes, LineSeg2D( cornerPoints[ 0 ], cornerPoints[ 2 ] ), color, thickness );
    AppendLine( vertexes, LineSeg2D( cornerPoints[ 1 ], cornerPoints[ 3 ] ), color, thickness );
}

//-----------------------------------------------------------------------------
void AppendDisc( std::vector<VertexMaster>& vertexes,
                 const Disc& disc,
                 const Rgba8& tint )
{
    const float degreesPerRadius = 360.f / CIRCLE_RADIUSES;
    Vec2 curr = Vec2::MakeFromPolarDegrees( 0.f, disc.radius ) + disc.center;

    // Start at the second radius because current is defined above.
    //  Loop up to <= DEBUG_CIRCLE_RADIUSES because 360 = 0 degrees
    //  This finishes the last group of triangles
    for ( int currentRadius = 1; currentRadius <= CIRCLE_RADIUSES; ++currentRadius )
    {
        const float currentDegrees = degreesPerRadius * static_cast<float>(currentRadius);
        Vec2 next = Vec2::MakeFromPolarDegrees( currentDegrees, disc.radius ) + disc.center;

        // First triangle
        vertexes.emplace_back( disc.center, tint );
        vertexes.emplace_back( curr, tint );
        vertexes.emplace_back( next, tint );

        curr = next;
    }
}

//-----------------------------------------------------------------------------
void AppendDiscPerimeter( std::vector<VertexMaster>& vertexes,
                          const Disc& disc,
                          const Rgba8& tint,
                          float thickness )
{
    const float degreesPerRadius = 360.f / CIRCLE_RADIUSES;
    const float lineRadius = thickness * .5f;
    Vec2 currShort = Vec2::MakeFromPolarDegrees( 0.f,
                                                 disc.radius - lineRadius ) + disc.center;
    Vec2 currLong = Vec2::MakeFromPolarDegrees( 0.f,
                                                disc.radius + lineRadius ) + disc.center;
    // Start at the second radius because current is defined above.
    //  Loop up to <= DEBUG_CIRCLE_RADIUSES because 360 = 0 degrees
    //  This finishes the last group of triangles
    for ( int currentRadius = 1; currentRadius <= CIRCLE_RADIUSES; ++currentRadius )
    {
        const float currentDegrees = degreesPerRadius * static_cast<float>(currentRadius);
        Vec2 nextShort = Vec2::MakeFromPolarDegrees( currentDegrees,
                                                     disc.radius - lineRadius ) + disc.center;
        Vec2 nextLong = Vec2::MakeFromPolarDegrees( currentDegrees,
                                                    disc.radius + lineRadius ) + disc.center;

        // Draw two circles per radius so 6 vertexes
        //  Because of the first definition of currShort is above the for
        //  minus one offset here

        // First triangle
        vertexes.emplace_back( currShort, tint );
        vertexes.emplace_back( currLong, tint );
        vertexes.emplace_back( nextShort, tint );
        // Second Triangle
        vertexes.emplace_back( nextShort, tint );
        vertexes.emplace_back( currLong, tint );
        vertexes.emplace_back( nextLong, tint );

        currShort = nextShort;
        currLong = nextLong;
    }
}

//-----------------------------------------------------------------------------
void AppendAABB2( std::vector<VertexMaster>& vertexes,
                  const AABB2& box,
                  const Rgba8& tint )
{
    Vec2 topLeft( box.mins.x, box.maxs.y );
    Vec2 botRight( box.maxs.x, box.mins.y );

    vertexes.emplace_back( box.mins, tint, Vec2( 0.f, 0.f ) );
    vertexes.emplace_back( box.maxs, tint, Vec2( 1.f, 1.f ) );
    vertexes.emplace_back( topLeft, tint, Vec2( 0.f, 1.f ) );
    vertexes.emplace_back( box.mins, tint, Vec2( 0.f, 0.f ) );
    vertexes.emplace_back( botRight, tint, Vec2( 1.f, 0.f ) );
    vertexes.emplace_back( box.maxs, tint, Vec2( 1.f, 1.f ) );
}

void AppendAABB2( std::vector<VertexMaster>& vertexes, const AABB2& box, const Rgba8& tint, const Vec2& uvMins, const Vec2& uvMaxs )
{
    Vec2 topLeft( box.mins.x, box.maxs.y );
    Vec2 botRight( box.maxs.x, box.mins.y );

    vertexes.emplace_back( box.mins, tint, uvMins );
    vertexes.emplace_back( box.maxs, tint, uvMaxs );
    vertexes.emplace_back( topLeft, tint, Vec2( uvMins.x, uvMaxs.y ) );
    vertexes.emplace_back( box.mins, tint, uvMins );
    vertexes.emplace_back( botRight, tint, Vec2( uvMaxs.x, uvMins.y ) );
    vertexes.emplace_back( box.maxs, tint, uvMaxs );
}

//-----------------------------------------------------------------------------
void AppendAABB2Perimeter( std::vector<VertexMaster>& vertexes,
                           const AABB2& box,
                           const Rgba8& tint,
                           float thickness )
{
    const Vec2 topLeft( box.mins.x, box.maxs.y );
    const Vec2 botRight( box.maxs.x, box.mins.y );

    AppendLine( vertexes, LineSeg2D( box.mins, topLeft ), tint, thickness );
    AppendLine( vertexes, LineSeg2D( topLeft, box.maxs ), tint, thickness );
    AppendLine( vertexes, LineSeg2D( box.maxs, botRight ), tint, thickness );
    AppendLine( vertexes, LineSeg2D( botRight, box.mins ), tint, thickness );
}

void AppendAABB3( std::vector<VertexMaster>& vertexes, const AABB3& cube, const Rgba8& tint )
{
    const Vec3 center = cube.GetCenter();
    const Vec3 dimens = cube.GetDimensions();

    const Vec3 frontOffset = Vec3( 0.f, 0.f, dimens.z );
    const Vec3 rightOffset = Vec3( dimens.x, 0.f, 0.f );
    const Vec3 topOffset = Vec3( 0.f, dimens.y, 0.f );

    const Vec3 iBasis = Vec3( dimens.x, 0.f, 0.f );
    const Vec3 jBasis = Vec3( 0.f, dimens.y, 0.f );
    const Vec3 kBasis = Vec3( 0.f, 0.f, dimens.z );

    AppendPlaneSegment( vertexes, center + frontOffset, iBasis, jBasis, tint );
    AppendPlaneSegment( vertexes, center + rightOffset, -kBasis, jBasis, tint );
    AppendPlaneSegment( vertexes, center - frontOffset, -iBasis, jBasis, tint );
    AppendPlaneSegment( vertexes, center - rightOffset, kBasis, jBasis, tint );
    AppendPlaneSegment( vertexes, center + topOffset, iBasis, -kBasis, tint );
    AppendPlaneSegment( vertexes, center - topOffset, iBasis, kBasis, tint );
}

void AppendCylinder( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes,
                     const LineSeg3D& lineSeg, const Rgba8& startTint, const Rgba8& endTint,
                     float startRadius, float endRadius, unsigned int sides )
{
    GUARANTEE_OR_DIE( sides >= 3, "AppendCylinder: There must be 3 or more sides" );
    constexpr unsigned int numVertexesPerStep = 10;

    const float theta = 360.f / static_cast<float>(sides);
    const Mat44 lineBasis = Mat44::LookAt( lineSeg.start, lineSeg.end, Vec3::J );
    const Vec3 right = lineBasis.GetIBasis3D();
    const Vec3 up = lineBasis.GetJBasis3D();

    int stepStartingIndex = static_cast<int>(vertexes.size());
    for ( unsigned int vertexIndex = 0; vertexIndex < sides; ++vertexIndex )
    {
        const float pointOneTheta = theta * static_cast<float>(vertexIndex);
        const float pointTwoTheta = theta * static_cast<float>(vertexIndex + 1);
        const Vec3 pointOneOffset = right * cosfDegrees( pointOneTheta ) + up * sinfDegrees(
            pointOneTheta );
        const Vec3 pointTwoOffset = right * cosfDegrees( pointTwoTheta ) + up * sinfDegrees(
            pointTwoTheta );

        const Vec3 circleTangent = (pointTwoOffset - pointOneOffset).GetNormalized();
        const Vec3 circleBitangent = (-pointOneOffset).GetNormalized();
        const Vec3 circleNormal = -lineBasis.GetKBasis3D();

        const float uvAtPointOne = RangeMapFloat( 0.f, 360.f, 0.f, 1.f, pointOneTheta );
        const float uvAtPointTwo = RangeMapFloat( 0.f, 360.f, 0.f, 1.f, pointTwoTheta );

        // Append the circles first
        vertexes.emplace_back( lineSeg.start, startTint, Vec2( uvAtPointOne, 0.f ), circleTangent, circleBitangent, -circleNormal  );
        vertexes.emplace_back( lineSeg.start + startRadius * pointOneOffset, startTint,
                               Vec2( uvAtPointOne, 1.f ), circleTangent, circleBitangent, -circleNormal );
        vertexes.emplace_back( lineSeg.start + startRadius * pointTwoOffset, startTint,
                               Vec2( uvAtPointTwo, 1.f ), circleTangent, circleBitangent, -circleNormal );

        // Wind the other way because its on the other side of the view
        vertexes.emplace_back( lineSeg.end, endTint, Vec2( uvAtPointOne, 0.f ), circleTangent, circleBitangent, circleNormal );
        vertexes.emplace_back( lineSeg.end + endRadius * pointTwoOffset, endTint,
                               Vec2( uvAtPointTwo, 1.f ), circleTangent, circleBitangent, circleNormal );
        vertexes.emplace_back( lineSeg.end + endRadius * pointOneOffset, endTint,
                               Vec2( uvAtPointOne, 1.f ), circleTangent, circleBitangent, circleNormal );

        // Append the cylinder quad
        vertexes.emplace_back( lineSeg.start + startRadius * pointOneOffset, startTint,
                               Vec2( 1.f, uvAtPointOne ), circleTangent, circleNormal, -circleBitangent );
        vertexes.emplace_back( lineSeg.end + endRadius * pointOneOffset, endTint,
                               Vec2( 0.f, uvAtPointOne ), circleTangent, circleNormal, -circleBitangent );
        vertexes.emplace_back( lineSeg.end + endRadius * pointTwoOffset, endTint,
                               Vec2( 0.f, uvAtPointTwo ), circleTangent, circleNormal, -circleBitangent );
        vertexes.emplace_back( lineSeg.start + startRadius * pointTwoOffset, startTint,
                               Vec2( 1.f, uvAtPointTwo ), circleTangent, circleNormal, -circleBitangent );

        // Append the indexes
        // Circles
        indexes.push_back( stepStartingIndex );
        indexes.push_back( stepStartingIndex + 1 );
        indexes.push_back( stepStartingIndex + 2 );
        indexes.push_back( stepStartingIndex + 3 );
        indexes.push_back( stepStartingIndex + 4 );
        indexes.push_back( stepStartingIndex + 5 );

        // Cylinder quads
        indexes.push_back( stepStartingIndex + 6 );
        indexes.push_back( stepStartingIndex + 7 );
        indexes.push_back( stepStartingIndex + 8 );
        indexes.push_back( stepStartingIndex + 6 );
        indexes.push_back( stepStartingIndex + 8 );
        indexes.push_back( stepStartingIndex + 9 );

        stepStartingIndex += numVertexesPerStep;
    }
}

void AppendCylinderPerimeter( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes, const LineSeg3D& lineSeg, const Rgba8& tint, float startRadius, float endRadius, float thickness, unsigned int sides )
{
    GUARANTEE_OR_DIE( sides >= 3, "AppendCylinder: There must be 3 or more sides" );
    constexpr unsigned int numVertexesPerStep = 10;

    const float theta = 360.f / static_cast<float>( sides );
    const Mat44 lineBasis = Mat44::LookAt( lineSeg.start, lineSeg.end, Vec3::J );
    const Vec3 right = lineBasis.GetIBasis3D();
    const Vec3 up = lineBasis.GetJBasis3D();

    for( unsigned int vertexIndex = 0; vertexIndex < sides; ++vertexIndex )
    {
        const float pointOneTheta = theta * static_cast<float>( vertexIndex );
        const float pointTwoTheta = theta * static_cast<float>( vertexIndex + 1 );
        const Vec3 pointOneOffset = right * cosfDegrees( pointOneTheta ) + up * sinfDegrees(
            pointOneTheta );
        const Vec3 pointTwoOffset = right * cosfDegrees( pointTwoTheta ) + up * sinfDegrees(
            pointTwoTheta );

        const float uvAtPointOne = RangeMapFloat( 0.f, 360.f, 0.f, 1.f, pointOneTheta );
        const float uvAtPointTwo = RangeMapFloat( 0.f, 360.f, 0.f, 1.f, pointTwoTheta );

        // Create rays between the circle points
        AppendRay( vertexes, indexes, LineSeg3D( lineSeg.start + startRadius * pointOneOffset, lineSeg.start + startRadius * pointTwoOffset ),
            tint, thickness );
        AppendRay( vertexes, indexes, LineSeg3D( lineSeg.end + endRadius * pointTwoOffset, lineSeg.end + endRadius * pointOneOffset ),
            tint, thickness );

        // Create the "vertical" rays between the start and end point of the cylinder
        AppendRay( vertexes, indexes, LineSeg3D( lineSeg.start + startRadius * pointOneOffset, lineSeg.end + endRadius * pointOneOffset ),
            tint, thickness );
    }
}

void AppendCone( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes, const LineSeg3D& lineSeg, const Rgba8& startTint, const Rgba8& endTint, float startRadius, unsigned int sides )
{
    AppendCylinder( vertexes, indexes, lineSeg, startTint, endTint, startRadius, 0.f, sides );
}

//-----------------------------------------------------------------------------
void AppendPolygon2D( std::vector<VertexMaster>& vertexes, const std::vector<Vec2>& points,
                      const Rgba8& color )
{
    const Vec2& zeroPoint = points.at( 0 );
    for ( size_t drawPointIndex = 1; drawPointIndex < points.size() - 1; ++drawPointIndex )
    {
        const Vec2& start = points.at( drawPointIndex );
        const Vec2& end = points.at( drawPointIndex + 1 );

        vertexes.emplace_back( zeroPoint, color );
        vertexes.emplace_back( start, color );
        vertexes.emplace_back( end, color );
    }
}

//-----------------------------------------------------------------------------
void AppendPolygon2DPerimeter( std::vector<VertexMaster>& vertexes, const std::vector<Vec2>& points,
                               const Rgba8& color, float thickness )
{
    for ( size_t drawPointIndex = 0; drawPointIndex < points.size() - 1; ++drawPointIndex )
    {
        AppendLine(
            vertexes, LineSeg2D( points.at( drawPointIndex ), points.at( drawPointIndex + 1 ) ),
            color,
            thickness );
    }
    AppendLine( vertexes, LineSeg2D( points.at( 0 ), points.at( points.size() - 1 ) ), color,
                thickness );
}

void AppendPlaneSegment( std::vector<VertexMaster>& vertexes, const Mat44& basis, const Rgba8& tint,
                         const Vec2& ijSize, const Vec2& pivot, const Vec2& uvAtMin,
                         const Vec2& uvAtMax )
{
    const Vec3 pivotTranslation = (ijSize.x * pivot.x * basis.GetIBasis3D()) + (ijSize.y * pivot.y *
        basis.GetJBasis3D());

    const Vec3 bottomLeft = basis.GetTranslation3D() - pivotTranslation;
    const Vec3 topLeft = bottomLeft + ijSize.y * basis.GetJBasis3D();
    const Vec3 topRight = bottomLeft + ijSize.x * basis.GetIBasis3D() + ijSize.y * basis.
        GetJBasis3D();
    const Vec3 bottomRight = bottomLeft + ijSize.x * basis.GetIBasis3D();

    vertexes.emplace_back( bottomLeft, tint, Vec2( uvAtMin.x, uvAtMax.y ) );
    vertexes.emplace_back( topRight, tint, Vec2( uvAtMax.x, uvAtMin.y ) );
    vertexes.emplace_back( topLeft, tint, uvAtMin );

    vertexes.emplace_back( bottomLeft, tint, Vec2( uvAtMin.x, uvAtMax.y ) );
    vertexes.emplace_back( bottomRight, tint, uvAtMax );
    vertexes.emplace_back( topRight, tint, Vec2( uvAtMax.x, uvAtMin.y ) );
}

void AppendPlaneSegment( std::vector<VertexMaster>& vertexes, const Vec3& center,
                         const Vec3& scaledIBasis, const Vec3& scaledJBasis, const Rgba8& tint,
                         const Vec2& uvAtMin, const Vec2& uvAtMax )
{
    const Vec3 halfI = scaledIBasis * .5f;
    const Vec3 halfJ = scaledJBasis * .5f;

    const Vec3 bottomLeft = center - scaledIBasis - scaledJBasis;
    const Vec3 topLeft = center - scaledIBasis + scaledJBasis;
    const Vec3 topRight = center + scaledIBasis + scaledJBasis;
    const Vec3 bottomRight = center + scaledIBasis - scaledJBasis;

    vertexes.emplace_back( bottomLeft, tint, Vec2( uvAtMin.x, uvAtMax.y ) );
    vertexes.emplace_back( topRight, tint, Vec2( uvAtMax.x, uvAtMin.y ) );
    vertexes.emplace_back( topLeft, tint, uvAtMin );

    vertexes.emplace_back( bottomLeft, tint, Vec2( uvAtMin.x, uvAtMax.y ) );
    vertexes.emplace_back( bottomRight, tint, uvAtMax );
    vertexes.emplace_back( topRight, tint, Vec2( uvAtMax.x, uvAtMin.y ) );
}
