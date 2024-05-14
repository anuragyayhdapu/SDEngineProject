#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.h"

#include <d3d11.h>

Texture::Texture()
{
}

Texture::~Texture()
{
	DX_SAFE_RELEASE(m_texture);
	DX_SAFE_RELEASE(m_sharedResourceView);
}
