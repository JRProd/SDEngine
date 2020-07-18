#include "ObjFileUtils.hpp"

#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"


#include "FileUtils.hpp"

enum class ObjLineType
{
    VERTEX,
    NORMAL,
    TEXTURE,
    FACE,

    MATERIAL,
    OBJECT,
    GROUP,

    SKIP,
};

static bool IsComment( const std::string& fileLine )
{
    const char firstCharacter = GetFirstNonWhitespaceCharacter( fileLine );
    return firstCharacter == '#';
}

static ObjLineType GetLineType( const std::string& lineBeginning )
{
    if ( lineBeginning == "v" )
    {
        return ObjLineType::VERTEX;
    }
    if ( lineBeginning == "vn" )
    {
        return ObjLineType::NORMAL;
    }
    if ( lineBeginning == "vt" )
    {
        return ObjLineType::TEXTURE;
    }
    if ( lineBeginning == "f" )
    {
        return ObjLineType::FACE;
    }
    if ( lineBeginning == "mtllib" )
    {
        return ObjLineType::MATERIAL;
    }
    if ( lineBeginning == "o" )
    {
        return ObjLineType::OBJECT;
    }
    if ( lineBeginning == "g" )
    {
        return ObjLineType::GROUP;
    }

    return ObjLineType::SKIP;
}

static void ParseVec3( const Strings& line, OUT_PARAM std::vector<Vec3>& vector )
{
    float x = ConvertFromString( line[ 1 ], 0.f );
    float y = ConvertFromString( line[ 2 ], 0.f );
    float z = ConvertFromString( line[ 3 ], 0.f );

    vector.emplace_back( x, y, z );
}

static void ParseVec2( const Strings& line, OUT_PARAM std::vector<Vec2>& vector )
{
    float x = ConvertFromString( line[ 1 ], 0.f );
    float y = ConvertFromString( line[ 2 ], 0.f );

    vector.emplace_back( x, y );
}

static VertexMaster MakeVertexMasterFromFaceSplit( const Strings& faceSplit,
                                                   const std::vector<Vec3>& objVertexes,
                                                   const std::vector<Vec2>& objUvs,
                                                   const MeshLoadOptions& loadOptions )
{
    const size_t vertexIndex = ConvertFromString( faceSplit[ 0 ], 0u ) - 1u;
    const size_t uvIndex = ConvertFromString( faceSplit[ 1 ], 0u ) - 1u;

    Vec2 uv = objUvs.at( uvIndex );
    if( loadOptions.invertV )
    {
        uv.y = 1 - uv.y;
    }


    return VertexMaster( loadOptions.transform.TransformPosition( objVertexes.at( vertexIndex ) ), 
                         Rgba8::WHITE, uv, Vec3::ONE, Vec3::ONE, Vec3::ONE );
}

static VertexMaster MakeVertexMasterFromFaceSplit( const Strings& faceSplit,
                                                   const std::vector<Vec3>& objVertexes,
                                                   const std::vector<Vec3>& objNormals,
                                                   const std::vector<Vec2>& objUvs,
                                                   const MeshLoadOptions& loadOptions )
{
    const size_t vertexIndex = ConvertFromString( faceSplit[ 0 ], 0u ) - 1u;
    const size_t uvIndex = ConvertFromString( faceSplit[ 1 ], 0u ) - 1u;
    const size_t normalIndex = ConvertFromString( faceSplit[ 2 ], 0u ) - 1u;

    Vec2 uv = objUvs.at( uvIndex );
    if ( loadOptions.invertV )
    {
        uv.y = 1 - uv.y;
    }

    return VertexMaster( loadOptions.transform.TransformPosition( objVertexes.at( vertexIndex ) ),
                         Rgba8::WHITE, uv, Vec3::ONE, Vec3::ONE,
                         loadOptions.transform.TransformVector( objNormals.at( normalIndex ) ).GetNormalized() );
}

static void ParseFaceCalculateNormals( OUT_PARAM std::vector<VertexMaster>& vertexes,
                                       const Strings& line,
                                       const std::vector<Vec3>& objVertexes,
                                       const std::vector<Vec2>& objUvs,
                                       const MeshLoadOptions& meshLoadOptions )
{
    Strings pointOneSplit = SplitStringOnDelimiter( line[ 1 ], '/' );
    VertexMaster pointOne = MakeVertexMasterFromFaceSplit( pointOneSplit, objVertexes, objUvs, 
                                                           meshLoadOptions );

    Vec3 normal = Vec3::ONE;
    bool firstRun = true;
    for ( size_t index = 2; index < line.size() - 1; ++index )
    {
        Strings pointTwoSplit = SplitStringOnDelimiter( line[ index ], '/' );
        Strings pointThreeSplit = SplitStringOnDelimiter( line[ index + 1 ], '/' );

        VertexMaster pointTwo = MakeVertexMasterFromFaceSplit( pointTwoSplit, objVertexes, objUvs,
                                                               meshLoadOptions );
        VertexMaster pointThree = MakeVertexMasterFromFaceSplit( pointThreeSplit, objVertexes, objUvs, 
                                                                 meshLoadOptions );

        if( firstRun )
        {
            // Calculate normal for this face
            const Vec3 pointOneToTwo = pointTwo.position - pointOne.position;
            const Vec3 pointOneToThree = pointThree.position - pointOne.position;
            normal = pointOneToTwo.GetCross( pointOneToThree ).GetNormalized();
        }

        // Push back the cornerstone point
        pointOne.normal = normal;
        vertexes.push_back( pointOne );

        // Always push back the third point
        pointTwo.normal = normal;
        pointThree.normal = normal;

        // Flip the winding order if requested
        if( meshLoadOptions.flipWindingOrder )
        {
            vertexes.push_back( pointThree );
            vertexes.push_back( pointTwo );
        }
        else
        {
            vertexes.push_back( pointTwo );
            vertexes.push_back( pointThree );
        }

        firstRun = false;
    }
}

static void ParseFaceGivenNormals( OUT_PARAM std::vector<VertexMaster>& vertexes,
                                   const Strings& line,
                                   const std::vector<Vec3>& objVertexes,
                                   const std::vector<Vec3>& objNormals,
                                   const std::vector<Vec2>& objUvs,
                                   const MeshLoadOptions& meshLoadOptions )
{
    Strings pointOneSplit = SplitStringOnDelimiter( line[ 1 ], '/' );
    VertexMaster pointOne = MakeVertexMasterFromFaceSplit( pointOneSplit, objVertexes, objNormals,
                                                           objUvs, meshLoadOptions );

    for ( size_t index = 2; index < line.size() - 1; ++index )
    {
        Strings pointTwoSplit = SplitStringOnDelimiter( line[ index ], '/' );
        Strings pointThreeSplit = SplitStringOnDelimiter( line[ index + 1 ], '/' );

        VertexMaster pointTwo = MakeVertexMasterFromFaceSplit(
            pointTwoSplit, objVertexes, objNormals, objUvs, meshLoadOptions );
        VertexMaster pointThree = MakeVertexMasterFromFaceSplit(
            pointThreeSplit, objVertexes, objNormals, objUvs, meshLoadOptions );

        // Push back the cornerstone point
        vertexes.push_back( pointOne );

        // Flit the winding order if requested
        if ( meshLoadOptions.flipWindingOrder )
        {
            vertexes.push_back( pointThree );
            vertexes.push_back( pointTwo );
        }
        else
        {
            vertexes.push_back( pointTwo );
            vertexes.push_back( pointThree );
        }
    }
}

static void ParseFace( OUT_PARAM std::vector<VertexMaster>& vertexes, const Strings& line,
                       const std::vector<Vec3>& objVertexes, const std::vector<Vec3>& objNormals,
                       const std::vector<Vec2>& objUvs,
                       const MeshLoadOptions& meshLoadOptions )
{

    if ( meshLoadOptions.calculateNormals )
    {
        ParseFaceCalculateNormals( vertexes, line, objVertexes, objUvs, meshLoadOptions );
    }
    else
    {
        ParseFaceGivenNormals( vertexes, line, objVertexes, objNormals, objUvs,
                               meshLoadOptions );
    }
}

static void ParseObjFile( const Strings& objFile, OUT_PARAM std::vector<VertexMaster>& vertexes,
                          OUT_PARAM std::vector<unsigned int>& indexes,
                          const MeshLoadOptions& meshLoadOptions )
{
    std::vector<Vec3> objVertexes;
    std::vector<Vec3> objNormals;
    std::vector<Vec2> objUvs;

    for ( const std::string& fileLine : objFile )
    {
        if ( IsComment( fileLine ) ) { continue; }

        Strings splitLine = SplitStringOnDelimiter( fileLine, ' ' );
        const ObjLineType type = GetLineType( splitLine[ 0 ] );

        switch ( type )
        {
        case ObjLineType::VERTEX:
            ParseVec3( splitLine, objVertexes );
            break;
        case ObjLineType::NORMAL:
            ParseVec3( splitLine, objNormals );
            break;
        case ObjLineType::TEXTURE:
            ParseVec2( splitLine, objUvs );
            break;
        case ObjLineType::FACE:
            ParseFace( vertexes, splitLine, objVertexes, objNormals, objUvs, 
                       meshLoadOptions );
            break;

        case ObjLineType::SKIP: // Explicit Fallthrough
        default:
            break;
        }
    }

    if( meshLoadOptions.calculateTangents )
    {
        MikkTangentCalculation( vertexes );
    }


    if( meshLoadOptions.clean )
    {
        CleanMesh( vertexes, indexes );
    }
    else
    {
        IndexMesh( vertexes, indexes );
    }
}

void LoadFromObjFile( const std::string& fileName, OUT_PARAM std::vector<VertexMaster>& vertexes,
                      OUT_PARAM std::vector<unsigned int>& indexes,
                      const MeshLoadOptions& meshLoadOptions )
{
    Strings file;
    FileReadToVector( fileName, file );

    ParseObjFile( file, vertexes, indexes, meshLoadOptions );
}
