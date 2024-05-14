#include "SpriteSheet.hpp"

SpriteSheet::SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout) :
	m_texture(texture), m_spriteDimensions(simpleGridLayout)
{
	SpliceSprite();
}

Texture& SpriteSheet::GetTexture() const
{
	return m_texture;
}

int SpriteSheet::GetNumSprites() const
{
	int numSprites = m_spriteDimensions.x * m_spriteDimensions.y;
	return numSprites;
}

SpriteDefinition const& SpriteSheet::GetSpriteDef(int spriteIndex) const
{
	SpriteDefinition const& spriteDef = m_spriteDefs[spriteIndex];
	return spriteDef;
}

void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const
{
	SpriteDefinition const& spriteDef = GetSpriteDef(spriteIndex);
	spriteDef.GetUVs(out_uvAtMins, out_uvAtMaxs);
}

AABB2 SpriteSheet::GetSpriteUVs(int spriteIndex) const
{
	SpriteDefinition const& spriteDef = GetSpriteDef(spriteIndex);
	AABB2 uv = spriteDef.GetUVs();
	return uv;
}

AABB2 SpriteSheet::GetSpriteUVs(IntVec2 spriteCoords) const
{
	int spriteIndex = (spriteCoords.y * m_spriteDimensions.y) + (spriteCoords.x);
	AABB2 uv = GetSpriteUVs(spriteIndex);
	return uv;
}

void SpriteSheet::SpliceSprite()
{
	// loop through sprite and create sprite dimensions

	int sprintIndex = 0;
	float uStep = 1.f / m_spriteDimensions.x;
	float vStep = 1.f / m_spriteDimensions.y;

	float samplingCorrectionX = 1 / (m_spriteDimensions.x * 128.f);
	float samplingCorrectionY = 1 / (m_spriteDimensions.y * 128.f);
	

	for (int y = m_spriteDimensions.y; y > 0; y--)
	{
		for (int x = 0; x < m_spriteDimensions.x; x++)
		{
			float uMin = (float)x / m_spriteDimensions.x + samplingCorrectionX;
			float uMax = uMin + uStep - samplingCorrectionX;

			float vMax = (float)y / m_spriteDimensions.y - samplingCorrectionY;
			float vMin = vMax - vStep + samplingCorrectionY;

			Vec2 uvAtMins = Vec2(uMin, vMin);
			Vec2 uvAtMaxs = Vec2(uMax, vMax);

			SpriteDefinition spriteDefinition(*this, sprintIndex, uvAtMins, uvAtMaxs);
			m_spriteDefs.push_back(spriteDefinition);

			sprintIndex++;
		}
	}

}
