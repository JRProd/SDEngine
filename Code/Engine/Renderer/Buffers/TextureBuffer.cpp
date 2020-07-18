#include "TextureBuffer.hpp"



#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"

TextureBuffer::TextureBuffer( RenderContext* owner )
    : m_Owner( owner )
{
}

void TextureBuffer::UpdateLocalBuffer( Texture* newTexture )
{
    if( m_CurrentTexture != newTexture )
    {
        m_CurrentTexture = newTexture;
        m_IsDirty = true;
    }
}

void TextureBuffer::MakeClean()
{
    m_IsDirty = false;
}

void TextureBuffer::UpdateAndBindBuffer( const unsigned int slot, const unsigned int numViews ) const
{
    if ( m_CurrentTexture == nullptr ) { return; }

    TextureView* view = m_CurrentTexture->CreateOrGetShaderResourceView();
    ID3D11ShaderResourceView* srv = view->GetAsSRV();

    m_Owner->m_Context->PSSetShaderResources( slot, numViews, &srv );
}
