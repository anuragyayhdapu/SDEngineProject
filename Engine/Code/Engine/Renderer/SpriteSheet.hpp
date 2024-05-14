#pragma once

#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.h"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <vector>

class SpriteSheet
{
public:
	explicit SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout);

	Texture&					GetTexture() const;
	int							GetNumSprites() const;
	SpriteDefinition const&		GetSpriteDef(int spriteIndex) const;
	void						GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const;
	AABB2						GetSpriteUVs(int spriteIndex) const;
	AABB2						GetSpriteUVs(IntVec2 spriteCoords) const;

protected:
	Texture&						m_texture;	// refernce members must be set in constructor initializer list
	std::vector<SpriteDefinition>	m_spriteDefs;
	IntVec2							m_spriteDimensions;

	void SpliceSprite();
};

