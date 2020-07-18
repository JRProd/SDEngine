#pragma once

#include <string>
#include <vector>

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"


class Image
{
public:
    explicit Image( const char* filePath );
    explicit Image( const std::string& filePath );
    Image( const Image& copy, int orthogonalRotation = 0, bool mirrored = false );

    void Create();
    void Destroy();

    //-------------------------------------------------------------------------
    // Image Accessors (const)
    const std::string GetImageFilePath() const { return m_FilePath; }
    const IntVec2 GetDimensions() const { return m_Dimension; }
    const Rgba8 GetTexelColor( int texelX, int texelY ) const;
    const Rgba8 GetTexelColor( const IntVec2& texelCoords ) const;

    //-------------------------------------------------------------------------
    // Image Modifiers (non-const)
    void SetTexelColor( int texelX, int texelY, const Rgba8& color );
    void SetTexelColor( const IntVec2& texelCoords, const Rgba8& color );

private:
    std::string m_FilePath;
    IntVec2 m_Dimension = IntVec2::ZERO;
    std::vector<Rgba8> m_Texels;

    int GetTexelIndexFromCoords( int texelX, int texelY ) const;
    int GetTexelIndexFromCoords( const IntVec2& texelCoords ) const;

    void ProcessRgbData( const unsigned char* imageData );
    void ProcessRgbaData( const unsigned char* imageData );
    void AddRgbaToVector( unsigned char r, 
                          unsigned char g, 
                          unsigned char b, 
                          unsigned char a = 255 );
};