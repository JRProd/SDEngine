#include "Image.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/stb/stb_image.h"

Image::Image( const char* filePath )
    : m_FilePath( filePath )
{
}

Image::Image( const std::string& filePath )
    : m_FilePath( filePath )
{
}

Image::Image( const Image& copy, int orthogonalRotation, bool mirrored )
    : m_FilePath( copy.m_FilePath )
{
    m_Texels.reserve( copy.m_Texels.size() );

    // Get the dimensions of the rotated version
    int rotation = static_cast<int>(GetAngleZeroTo360( static_cast<float>(orthogonalRotation) ));
    if ( orthogonalRotation == 0 || orthogonalRotation == 180 )
    {
        m_Dimension = copy.m_Dimension;
    }
    else
    {
        m_Dimension = IntVec2( copy.m_Dimension.y, copy.m_Dimension.x );
    }

    // Copy data based on the 8 possible orientations and mirrored directions
    if ( !mirrored )
    {
        switch ( rotation )
        {
            case 0:
                m_Texels = copy.m_Texels;
                break;
            case 90:
                for ( int y = 0; y < m_Dimension.y; ++y )
                {
                    for ( int x = m_Dimension.x - 1; x >= 0; --x )
                    {
                        int vectorPosition = x * m_Dimension.y + y;
                        m_Texels.push_back( copy.m_Texels.at( vectorPosition ) );
                    }
                }
                break;
            case 180:
                for ( int y = copy.m_Dimension.y - 1; y >= 0; --y )
                {
                    for ( int x = copy.m_Dimension.x - 1; x >= 0; --x )
                    {
                        int vectorPosition = y * copy.m_Dimension.x + x;
                        m_Texels.push_back( copy.m_Texels.at( vectorPosition ) );
                    }
                }
                break;
            case 270:
                for ( int y = m_Dimension.y - 1; y >= 0; --y )
                {
                    for ( int x = 0; x < m_Dimension.x; ++x )
                    {
                        int vectorPosition = x * m_Dimension.y + y;
                        m_Texels.push_back( copy.m_Texels.at( vectorPosition ) );
                    }
                }
                break;
            default:
            #if !defined(ENGINE_DISABLE_CONSOLE)
                g_Console->LogWTF( Stringf( "Image: ERROR Orientation %d is not orthogonal", orthogonalRotation ) );
            #endif // !defined(ENGINE_DISABLE_CONSOLE)
            #if defined(ENGINE_DISABLE_CONSOLE)
                ERROR_AND_DIE( Stringf( "Image: ERROR Orientation %d is not orthoganal", orthogonalRotation ) );
            #endif // defined(ENGINE_DISABLE_CONSOLE)
                break;
        }
    }
    else
    {
        switch ( rotation )
        {
            case 0:
                for ( int y = 0; y < m_Dimension.y; ++y )
                {
                    for ( int x = m_Dimension.x - 1; x >= 0; --x )
                    {
                        int vectorPosition = y * m_Dimension.x + x;
                        m_Texels.push_back( copy.m_Texels.at( vectorPosition ) );
                    }
                }
                break;
            case 90:
                for ( int y = m_Dimension.y - 1; y >= 0; --y )
                {
                    for ( int x = m_Dimension.x - 1; x >= 0; --x )
                    {
                        int vectorPosition = x * m_Dimension.y + y;
                        m_Texels.push_back( copy.m_Texels.at( vectorPosition ) );
                    }
                }
                break;
            case 180:
                for ( int y = m_Dimension.y - 1; y >= 0; --y )
                {
                    for ( int x = 0; x < m_Dimension.x; x++ )
                    {
                        int vectorPosition = y * m_Dimension.x + x;
                        m_Texels.push_back( copy.m_Texels.at( vectorPosition ) );
                    }
                }
                break;
            case 270:
                for ( int y = 0; y < m_Dimension.y; y++ )
                {
                    for ( int x = 0; x < m_Dimension.x; ++x )
                    {
                        int vectorPosition = x * m_Dimension.y + y;
                        m_Texels.push_back( copy.m_Texels.at( vectorPosition ) );
                    }
                }
                break;
            default:
            #if !defined(ENGINE_DISABLE_CONSOLE)
                g_Console->LogWTF( Stringf( "Image: ERROR Orientation %d is not orthogonal", orthogonalRotation ) );
            #endif // !defined(ENGINE_DISABLE_CONSOLE)
            #if defined(ENGINE_DISABLE_CONSOLE)
                ERROR_AND_DIE( Stringf( "Image: ERROR Orientation %d is not orthoganal", orthogonalRotation ) );
            #endif // defined(ENGINE_DISABLE_CONSOLE)
                break;
        }
    }
}


void Image::Create()
{
    int texelSizeX = 0;
    int texelSizeY = 0;
    int numComponents = 0;
    int numComponentsRequested = 0;

    stbi_set_flip_vertically_on_load( 1 );
    unsigned char* imageData = stbi_load( m_FilePath.c_str(),
                                          &texelSizeX,
                                          &texelSizeY,
                                          &numComponents,
                                          numComponentsRequested );

    // Ensure that the data was correctly read in
#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->GuaranteeOrWTF( imageData, Stringf( "Failed too load image \"%s\"", m_FilePath.c_str() ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)
#if defined(ENGINE_DISABLE_CONSOLE)
    GUARANTEE_OR_DIE( imageData, Stringf( "Failed to load image \"%s\"", m_FilePath.c_str() ) );
#endif // defined(ENGINE_DISABLE_CONSOLE)

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->GuaranteeOrWTF( (numComponents >= 3 && numComponents <= 4 && texelSizeX > 0 && texelSizeY > 0),
                               Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)",
                                        m_FilePath.c_str(),
                                        numComponents,
                                        texelSizeX,
                                        texelSizeY ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)
#if defined(ENGINE_DISABLE_CONSOLE)
    GUARANTEE_OR_DIE( (numComponents >= 3 && numComponents <= 4 && texelSizeX > 0 && texelSizeY > 0),
                      Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)",
                               m_FilePath.c_str(),
                               numComponents,
                               texelSizeX,
                               texelSizeY ) );
#endif // defined(ENGINE_DISABLE_CONSOLE)

    m_Dimension = IntVec2( texelSizeX, texelSizeY );

    if ( numComponents == 3 )
    {
        ProcessRgbData( imageData );
    }
    else if ( numComponents == 4 )
    {
        ProcessRgbaData( imageData );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_VERBOSE, Stringf( "Image: Successfully loaded image \"%s\" (Bpp=%i, size=%i,%i)",
                                          m_FilePath.c_str(),
                                          numComponents,
                                          texelSizeX,
                                          texelSizeY ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)
}

void Image::Destroy()
{
}

const Rgba8 Image::GetTexelColor( int texelX, int texelY ) const
{
    return m_Texels.at( GetTexelIndexFromCoords( texelX, texelY ) );
}

const Rgba8 Image::GetTexelColor( const IntVec2& texelCoords ) const
{
    return m_Texels.at( GetTexelIndexFromCoords( texelCoords ) );
}

void Image::SetTexelColor( int texelX, int texelY, const Rgba8& color )
{
    m_Texels[ GetTexelIndexFromCoords( texelX, texelY ) ] = color;
}

void Image::SetTexelColor( const IntVec2& texelCoords, const Rgba8& color )
{
    m_Texels[ GetTexelIndexFromCoords( texelCoords ) ] = color;
}

int Image::GetTexelIndexFromCoords( int texelX, int texelY ) const
{
    return texelY * m_Dimension.x + texelX;
}

int Image::GetTexelIndexFromCoords( const IntVec2& texelCoords ) const
{
    return texelCoords.y * m_Dimension.x + texelCoords.x;
}

void Image::ProcessRgbData( const unsigned char* imageData )
{
    int numTexelData = m_Dimension.x * m_Dimension.y * 3;
    for ( int currentTexel = 0; currentTexel < numTexelData; currentTexel += 3 )
    {
        unsigned char r = imageData[ currentTexel ];
        unsigned char g = imageData[ currentTexel + 1 ];
        unsigned char b = imageData[ currentTexel + 2 ];
        AddRgbaToVector( r, g, b );
    }
}

void Image::ProcessRgbaData( const unsigned char* imageData )
{
    int numTexelData = m_Dimension.x * m_Dimension.y * 4;
    for ( int currentTexel = 0; currentTexel < numTexelData; currentTexel += 4 )
    {
        unsigned char r = imageData[ currentTexel ];
        unsigned char g = imageData[ currentTexel + 1 ];
        unsigned char b = imageData[ currentTexel + 2 ];
        unsigned char a = imageData[ currentTexel + 3 ];
        AddRgbaToVector( r, g, b, a );
    }
}

void Image::AddRgbaToVector( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
    m_Texels.push_back( Rgba8( r, g, b, a ) );
}
