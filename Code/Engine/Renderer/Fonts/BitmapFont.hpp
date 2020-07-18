#pragma once
#include <string>
#include <vector>

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"

#include "Engine/Renderer/Sprite/SpriteSheet.hpp"

struct Mat44;
struct Vec2;
class RenderContext;
class Texture;

constexpr int SPACE_PER_TAB = 4;

class BitmapFont
{
    friend class RenderContext;
public:

    const Texture* GetTexture() const;

    void Render( RenderContext& renderer,
                 std::string& text,
                 const Vec2& textMins,
                 float cellHeight,
                 const Rgba8& tint = Rgba8::WHITE,
                 float cellAspect = .56f ) const;

    void RenderInBox( RenderContext& renderer,
                      const std::string& text,
                      const AABB2& box,
                      float cellHeight,
                      const Vec2& alignment = Vec2::ALIGN_CENTERED,
                      const Rgba8& tint = Rgba8::WHITE,
                      float cellAspect = .56f ) const;

    void AddVertsForText( std::vector<VertexMaster>& vertexArray,
                          const std::string& text,
                          const Vec2& origin,
                          const Vec2& pivot,
                          float cellHeight,
                          const Rgba8& tint = Rgba8::WHITE,
                          float cellAspect = .56f ) const;
    void AddVertsForText( std::vector<VertexMaster>& vertexArray,
                          const std::string& text,
                          const Vec2& origin,
                          float cellHeight,
                          const Rgba8& tint = Rgba8::WHITE,
                          float cellAspect = .56f ) const;

    void AddVertsForText3D( std::vector<VertexMaster>& vertexArray,
                            const std::string& text,
                            const Mat44& basis,
                            const Vec2& pivot,
                            float cellHeight,
                            const Rgba8& tint = Rgba8::WHITE,
                            float cellAspect = .56f ) const;

    void AddVertsForTextInBox( std::vector<VertexMaster>& vertexArray,
                               const std::string& text,
                               const AABB2& box,
                               float cellHeight,
                               const Vec2& alignment = Vec2::ALIGN_CENTERED,
                               const Rgba8& tint = Rgba8::WHITE,
                               float cellAspect = .56f ) const;

    void AddVertsForMarkedTextInBox( std::vector<VertexMaster>& vertexArray,
                                     const std::string& text,
                                     const AABB2& box,
                                     float cellHeight,
                                     const Vec2& alignment = Vec2::ALIGN_CENTERED,
                                     Rgba8 tint = Rgba8::WHITE,
                                     float cellAspect = .56f ) const;

    void AddVertsForMarkedText( std::vector<VertexMaster>& vertexArray,
                                const std::string& text,
                                const Vec2& textMins,
                                float cellHeight,
                                const Rgba8& tint = Rgba8::WHITE,
                                float cellAspect = .56f ) const;

    Vec2 GetDimensionForText( const std::string& text,
                              float cellHeight,
                              float cellAspect = .56f ) const;

    Vec2 GetDimensionForMarkedText( const std::string& text,
                                    float cellHeight,
                                    float cellAspect = .56f ) const;

    float GetGlyphAspect( unsigned char glyphUnicode ) const;
    float GetAspect() const { return m_DefaultAspect; }

protected:
    const std::string m_FontName;
    const SpriteSheet m_FontSheet;

    float m_BorderWidth = 0.f;
    float m_DefaultAspect = .56f;

private:
    BitmapFont( const char* fontName,
                const Texture* fontTexture,
                float aspect = .56f );

    void AppendCharacterToVector( std::vector<VertexMaster>& vertexArray,
                                  unsigned char character,
                                  const Vec2& characterBottomLeft,
                                  float cellHeight,
                                  const Rgba8& tint = Rgba8::WHITE,
                                  float cellAspect = 1.f ) const;
    void AppendCharacterToVector3D( std::vector<VertexMaster>& vertexArray,
                                  unsigned char character,
                                  const Mat44& basis,
                                  float cellHeight,
                                  const Rgba8& tint = Rgba8::WHITE,
                                  float cellAspect = 1.f ) const;
};
