#pragma once

#include <string>
#include <vector>

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"

class Texture;
class Vertex_PCU;
struct Vec2;

enum class TextBoxMode
{
	SHRINK,
	OVERRUN
};

//------------------------------------------------------------------------------------------------
// Facilitate ASCII string text drawing using a glyphs sprite sheet texture
class BitmapFont
{
	friend class Renderer; // Only the Renderer can create new BitmapFont objects!

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture);

public:
	Texture& GetTexture();

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f);
	float GetTextWidth(float cellHeight, std::string const& text, float cellAspect = 1.f);
	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return 1.0f!!!

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;

public:
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f, Vec2 const& alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK, int maxGlyphsToDraw = 99999999);
	float GetTextHeight(float cellHeight, std::string const& text, float cellAspect = 1.f);
	// 3d text
	void AddVertsForText3D(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f, Vec2 const& alignment = Vec2(0.5f, 0.5f), int maxGlyphToDraw = 999999999);
};