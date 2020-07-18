#pragma once

class Texture;
class RenderContext;

class TextureBuffer
{
    friend class RenderContext;

public:
    TextureBuffer( RenderContext* owner );

    bool IsDirty() const { return m_IsDirty; }
    void UpdateLocalBuffer( Texture* newTexture );

    void MakeClean();

private:
    RenderContext* m_Owner = nullptr;

    Texture* m_CurrentTexture = nullptr;
    bool m_IsDirty = true;

    void UpdateAndBindBuffer( unsigned int slot, unsigned int numViews = 1 ) const;
};
