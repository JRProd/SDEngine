#include "BitmapFont.hpp"

#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Core/Utils/VectorPcuUtils.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Sprite/SpriteSheet.hpp"


const Texture* BitmapFont::GetTexture() const
{
    return &m_FontSheet.GetTexture();
}

void BitmapFont::Render( RenderContext& renderer,
                         std::string& text,
                         const Vec2& textMins,
                         const float cellHeight,
                         const Rgba8& tint,
                         const float cellAspect ) const
{
    std::vector<VertexMaster> textVisual;

    AddVertsForText( textVisual, text, textMins, cellHeight, tint, cellAspect );
    renderer.BindTexture( &m_FontSheet.GetTexture() );
    renderer.DrawVertexArray( textVisual );
}

void BitmapFont::RenderInBox( RenderContext& renderer,
                              const std::string& text,
                              const AABB2& box,
                              const float cellHeight,
                              const Vec2& alignment,
                              const Rgba8& tint,
                              const float cellAspect ) const
{
    std::vector<VertexMaster> textVisual;

    AddVertsForTextInBox( textVisual,
                          text,
                          box,
                          cellHeight,
                          alignment,
                          tint,
                          cellAspect );

    renderer.BindTexture( &m_FontSheet.GetTexture() );
    renderer.DrawVertexArray( textVisual );
}

void BitmapFont::AddVertsForText( std::vector<VertexMaster>& vertexArray, const std::string& text,
                                  const Vec2& origin, const Vec2& pivot, const float cellHeight,
                                  const Rgba8& tint, const float cellAspect ) const
{
    Vec2 textDimensions = GetDimensionForText( text, cellHeight, cellAspect );
    // Remove the first line for the dimension calculation
    textDimensions.y -= cellHeight;
    const Vec2 characterSizeX = Vec2( cellHeight * cellAspect, 0.f );

    Vec2 characterPos = origin;
    characterPos.x -= textDimensions.x * pivot.x;

    characterPos.y -= textDimensions.y * (1 - pivot.y) +(cellHeight * pivot.y);

    for ( const unsigned char& c : text )
    {
        if ( c == '\n' )
        {
            characterPos = Vec2( origin.x, characterPos.y - cellHeight );
            continue;
        }
        if ( c == '\t' )
        {
            characterPos.x += cellHeight * cellAspect * SPACE_PER_TAB;
            continue;
        }

        AppendCharacterToVector( vertexArray,
                                 c,
                                 characterPos,
                                 cellHeight,
                                 tint,
                                 cellAspect );

        characterPos += characterSizeX;
    }
}

void BitmapFont::AddVertsForText( std::vector<VertexMaster>& vertexArray,
                                  const std::string& text,
                                  const Vec2& origin,
                                  const float cellHeight,
                                  const Rgba8& tint,
                                  const float cellAspect ) const
{
    AddVertsForText( vertexArray, text, origin, Vec2::ALIGN_BOTTOM_LEFT, cellHeight, tint,
                     cellAspect );
}

void BitmapFont::AddVertsForText3D( std::vector<VertexMaster>& vertexArray, const std::string& text,
                                    const Mat44& basis, const Vec2& pivot, const float cellHeight,
                                    const Rgba8& tint, const float cellAspect ) const
{
    Mat44 currentCharBasis = basis;
    Vec3 characterPos = currentCharBasis.GetTranslation3D();
    const Vec2 textDimensions = GetDimensionForText( text, cellHeight, cellAspect );
    characterPos -= currentCharBasis.GetIBasis3D() * textDimensions.x * pivot.x;
    characterPos -= currentCharBasis.GetJBasis3D() * textDimensions.y * pivot.y;
    const Vec3 characterSizeX = currentCharBasis.GetIBasis3D() * cellHeight * cellAspect;
    const Vec3 characterSizeY;

    currentCharBasis.SetTranslation( characterPos );

    for ( const unsigned char& c : text )
    {
        AppendCharacterToVector3D( vertexArray, c, currentCharBasis, cellHeight, tint, cellAspect );

        characterPos += characterSizeX;
        currentCharBasis.SetTranslation( characterPos );
    }
}

void BitmapFont::AddVertsForTextInBox( std::vector<VertexMaster>& vertexArray,
                                       const std::string& text,
                                       const AABB2& box,
                                       const float cellHeight,
                                       const Vec2& alignment,
                                       const Rgba8& tint,
                                       const float cellAspect ) const
{
    // Always positive
    AABB2 textDimension = AABB2( Vec2::ZERO, GetDimensionForText( text, cellHeight, cellAspect ) );
    textDimension.AlignWithinBounds( box, alignment );

    Vec2 textPosition = textDimension.GetTopLeft();
    textPosition.y -= cellHeight;

    // If the text is only one line
    if( textDimension.GetDimensions().y == cellHeight )
    {
    }

    const float characterSizeX = cellHeight * cellAspect;
    Vec2 characterPosition = textPosition;
    for ( const unsigned char& c : text )
    {
        if ( c == '\n' )
        {
            characterPosition = Vec2( textPosition.x, characterPosition.y - cellHeight );
            continue;
        }
        if ( c == '\t' )
        {
            characterPosition.x += cellHeight * cellAspect * SPACE_PER_TAB;
            continue;
        }

        AppendCharacterToVector( vertexArray,
                                 c,
                                 characterPosition,
                                 cellHeight,
                                 tint,
                                 cellAspect );

        characterPosition.x += characterSizeX;
    }
}

void BitmapFont::AddVertsForMarkedTextInBox( std::vector<VertexMaster>& vertexArray,
                                             const std::string& text, const AABB2& box,
                                             float cellHeight, const Vec2& alignment,
                                             const Rgba8 tint, float cellAspect ) const
{
    // Always positive
    Vec2 textDimension = GetDimensionForMarkedText( text, cellHeight, cellAspect );
    // Always <= box.GetDimensions
    Vec2 remainingBoxDimension = box.GetDimensions() - textDimension;
    // Clamp Vec2 between (0,0) and box.GetDimensions
    //     remainingBoxDimension = Vec2::ClampVector( remainingBoxDimension, 
    //                                                Vec2( 0.f, 0.f ), 
    //                                                box.GetDimensions() );

    // Interpolate between (0, 0) and remainingBoxDimension by the alignment
    //  Then add the bottom left box position
    Vec2 textPosition = box.mins + remainingBoxDimension * alignment;
    textPosition.y += -cellHeight + textDimension.y;

    AddVertsForMarkedText( vertexArray,
                           text,
                           textPosition,
                           cellHeight,
                           tint,
                           cellAspect );
}

void BitmapFont::AddVertsForMarkedText( std::vector<VertexMaster>& vertexArray,
                                        const std::string& text,
                                        const Vec2& textMins, float cellHeight,
                                        const Rgba8& tint, float cellAspect ) const
{
    Vec2 characterPos = textMins;
    Vec2 characterSizeX = Vec2( cellHeight * cellAspect, 0.f );
    MarkedStrings markedStrings = ParseMarkedString( text );
    for ( const MarkedString& marked : markedStrings )
    {
        Rgba8 textColor = tint;
        if ( marked.isMarked )
        {
            textColor = marked.info.color;
        }

        for ( const unsigned char& c : marked.string )
        {
            if ( c == '\n' )
            {
                characterPos = Vec2( textMins.x, characterPos.y - cellHeight );
                continue;
            }
            if ( c == '\t' )
            {
                characterPos.x += cellHeight * cellAspect * SPACE_PER_TAB;
                continue;
            }

            AppendCharacterToVector( vertexArray,
                                     c,
                                     characterPos,
                                     cellHeight,
                                     textColor,
                                     cellAspect );

            characterPos += characterSizeX;
        }
    }
}

Vec2 BitmapFont::GetDimensionForText( const std::string& text,
                                      float cellHeight,
                                      float cellAspect ) const
{
    // Store the current text length, is reset every new line
    float currentTextLength = 0.f;
    Vec2 textDimensions = Vec2( 0.f, cellHeight );
    for ( const unsigned char& c : text )
    {
        if ( c == '\n' )
        {
            currentTextLength = 0.f;
            textDimensions.y += cellHeight;
        }
        else
        {
            currentTextLength += cellHeight * cellAspect * GetGlyphAspect( c );

            // Store the max currentTextLength in the textDimensions.x
            if ( currentTextLength > textDimensions.x )
            {
                textDimensions.x = currentTextLength;
            }
        }
    }

    return textDimensions;
}

Vec2 BitmapFont::GetDimensionForMarkedText( const std::string& text, float cellHeight,
                                            float cellAspect ) const
{
    MarkedStrings markedStrings = ParseMarkedString( text );
    std::string unmarkedString;
    for ( const MarkedString& marked : markedStrings )
    {
        unmarkedString += marked.string;
    }
    return GetDimensionForText( unmarkedString, cellHeight, cellAspect );
}

float BitmapFont::GetGlyphAspect( unsigned char glyphUnicode ) const
{
    UNUSED( glyphUnicode );
    return 1.f;
}

BitmapFont::BitmapFont( const char* fontName, const Texture* fontTexture, float aspect )
    : m_FontName( fontName )
      , m_FontSheet( *fontTexture, IntVec2( 16, 16 ) )
      , m_DefaultAspect( aspect )
{
}

void BitmapFont::AppendCharacterToVector( std::vector<VertexMaster>& vertexArray,
                                          unsigned char character,
                                          const Vec2& characterBottomLeft,
                                          float cellHeight,
                                          const Rgba8& tint,
                                          float cellAspect ) const
{
    // Make a unit box based on the aspect ratio of the character
    AABB2 characterBox = AABB2::UNIT_BOX;
    const float cellWidth = cellHeight * GetGlyphAspect( character ) * cellAspect;
    characterBox.ScaleFromMin( Vec2( cellWidth, cellHeight ) );

    // Move the box to the character position
    characterBox.Translate( characterBottomLeft );
    const AABB2 characterBoardRight = characterBox.GetTranslation( Vec2( 1, 0 ) * m_BorderWidth );
    const AABB2 characterBoardTop = characterBox.GetTranslation( Vec2( 0, 1 ) * m_BorderWidth );
    const AABB2 characterBoardLeft = characterBox.GetTranslation( Vec2( -1, 0 ) * m_BorderWidth );
    const AABB2 characterBoardBottom = characterBox.GetTranslation( Vec2( 0, -1 ) * m_BorderWidth );

    Vec2 uvMin = Vec2::ZERO;
    Vec2 uvMax = Vec2::ONE;

    m_FontSheet.GetSpriteDefinition( character ).GetUVs( uvMin, uvMax );

    if( m_BorderWidth > 0 )
    {
        AppendAABB2( vertexArray, characterBoardRight, Rgba8::BLACK, uvMin, uvMax );
        AppendAABB2( vertexArray, characterBoardTop, Rgba8::BLACK, uvMin, uvMax );
        AppendAABB2( vertexArray, characterBoardLeft, Rgba8::BLACK, uvMin, uvMax );
        AppendAABB2( vertexArray, characterBoardBottom, Rgba8::BLACK, uvMin, uvMax );
    }
    AppendAABB2( vertexArray, characterBox, tint, uvMin, uvMax );
}

void BitmapFont::AppendCharacterToVector3D( std::vector<VertexMaster>& vertexArray,
                                            const unsigned char character,
                                            const Mat44& basis, const float cellHeight,
                                            const Rgba8& tint,
                                            const float cellAspect ) const
{
    // Vec3 centerPoint = basis.GetTranslation3D();    // Bottom Left
    // const Vec3 scaledIBasis = basis.GetIBasis3D() * cellHeight * cellAspect;
    // const Vec3 scaledJBasis = basis.GetJBasis3D() * cellHeight;
    // centerPoint += scaledIBasis * .5f;
    // centerPoint += scaledJBasis * .5f;

    Vec2 uvMin = Vec2::ZERO;
    Vec2 uvMax = Vec2::ONE;

    m_FontSheet.GetSpriteDefinition( character ).GetUVs( uvMin, uvMax );

    if ( m_BorderWidth > 0 )
    {
        Mat44 rightBasis = basis;
        rightBasis.SetTranslation( rightBasis.GetTranslation3D() + rightBasis.GetIBasis3D() * m_BorderWidth * .01f );
        AppendPlaneSegment( vertexArray, rightBasis, Rgba8::BLACK, Vec2( cellHeight * cellAspect, cellHeight ),
                            Vec2::ZERO, uvMin, uvMax );

        Mat44 topBasis = basis;
        topBasis.SetTranslation( topBasis.GetTranslation3D() + topBasis.GetJBasis3D() * m_BorderWidth * .01f );
        AppendPlaneSegment( vertexArray, topBasis, Rgba8::BLACK, Vec2( cellHeight * cellAspect, cellHeight ),
                            Vec2::ZERO, uvMin, uvMax );

        Mat44 leftBasis = basis;
        leftBasis.SetTranslation( leftBasis.GetTranslation3D() + leftBasis.GetIBasis3D() * -m_BorderWidth * .01f );
        AppendPlaneSegment( vertexArray, leftBasis, Rgba8::BLACK, Vec2( cellHeight * cellAspect, cellHeight ),
                            Vec2::ZERO, uvMin, uvMax );

        Mat44 bottomBasis = basis;
        bottomBasis.SetTranslation( bottomBasis.GetTranslation3D() + bottomBasis.GetJBasis3D() * -m_BorderWidth * .01f );
        AppendPlaneSegment( vertexArray, bottomBasis, Rgba8::BLACK, Vec2( cellHeight * cellAspect, cellHeight ),
                            Vec2::ZERO, uvMin, uvMax );
    }
    AppendPlaneSegment( vertexArray, basis, tint, Vec2( cellHeight * cellAspect, cellHeight ),
                        Vec2::ZERO, uvMin, uvMax );
}
