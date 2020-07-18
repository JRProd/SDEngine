#include "Engine/Renderer/Fonts/BitmapFont.hpp"
struct Mat44;

namespace Engine
{
    void SetCanonicalTransform( const Mat44& gameTransform );

    const Mat44& GetCanonicalTransform();
    const Mat44& GetCanonicalTransformInverse();
    const Mat44& GetCanonicalTransformTranspose();

    extern BitmapFont* g_Font;
}
