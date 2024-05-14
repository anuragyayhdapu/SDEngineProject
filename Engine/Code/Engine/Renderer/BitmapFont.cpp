#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture)
	: m_fontFilePathNameWithNoExtension(fontFilePathNameWithNoExtension)
	, m_fontGlyphsSpriteSheet(SpriteSheet(fontTexture, IntVec2(16, 16)))
{
}

Texture& BitmapFont::GetTexture()
{
	Texture& fontTexture = m_fontGlyphsSpriteSheet.GetTexture();
	return fontTexture;
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray,
	Vec2 const& textMins, float cellHeight, std::string const& text,
	Rgba8 const& tint, float cellAspect)
{
	float cellWidth = cellAspect * cellHeight;
	// like getting a texture from tile sprite sheet

	// add vert for each character in text one by one
	for (int textIndex = 0; textIndex < text.size(); textIndex++) // for each character
	{
		int fontIndex = text[textIndex]; // 1. get the ascii code
		AABB2 uv = m_fontGlyphsSpriteSheet.GetSpriteUVs(fontIndex); // 2. get uvs from spritesheet for this ascii code
		// 3. get the bound for this character
		// for the first char only
		Vec2 mins(textMins.x + (textIndex * cellWidth), textMins.y);
		Vec2 maxs(mins.x + cellWidth, mins.y + cellHeight);
		AABB2 bounds(mins, maxs);

		// 4. add vert for this char using abb2 bounds
		AddVertsForAABB2(vertexArray, bounds, tint, uv.m_mins, uv.m_maxs);
	}
}

float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspect)
{
	// split the string on delimiter
	Strings lines = SplitStringOnDelimiter(text, '\n');
	int widestLineSize = 0;

	// get the size of widest line
	for (int index = 0; index < lines.size(); index++)
	{
		int linesize = (int)lines[index].size();
		if (widestLineSize < linesize)
			widestLineSize = linesize;
	}

	float cellWidth = cellAspect * cellHeight;
	

	float textWidth = cellWidth * widestLineSize;
	return textWidth;
}

// For now this will always return 1.0f!!!
float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);

	return 1.0f;
}

void BitmapFont::AddVertsForTextInBox2D
(
	std::vector<Vertex_PCU>&			vertexArray,
	AABB2						const&	box,
	float								cellHeight,
	std::string					const&	text,
	Rgba8						const&	tint,
	float								cellAspect,
	Vec2						const&	alignment,
	TextBoxMode							mode,
	int									maxGlyphsToDraw
)
{
	float boxWidth = box.m_maxs.x - box.m_mins.x;
	float boxHeight = box.m_maxs.y - box.m_mins.y;

	if (mode == TextBoxMode::SHRINK)
	{
		float textWidth = GetTextWidth(cellHeight, text, cellAspect);
		if (textWidth > boxWidth)
		{
			float overflowWidth = textWidth - boxWidth;
			float overflowPercent = overflowWidth / textWidth;

			cellHeight -= (cellHeight * overflowPercent);
		}

		float textHeight = GetTextHeight(cellHeight, text, cellAspect);
		if (textHeight > boxHeight)
		{
			float overflowHeight = textHeight - boxHeight;
			float overflowPercent = overflowHeight / textHeight;

			cellHeight -= (cellHeight * overflowPercent);
		}
	}

	std::string textToDraw = text;
	if (maxGlyphsToDraw < text.size())
	{
		int totalSpecialChars = 0;
		for (size_t index = 0; index < maxGlyphsToDraw; index++)
		{
			char c = text[index];
			if (c == '\n')
			{
				totalSpecialChars++;
			}
		}

		maxGlyphsToDraw += totalSpecialChars;
		textToDraw = text.substr(0, maxGlyphsToDraw);
	}

	float textHeight = GetTextHeight(cellHeight, text);

	Strings lineSeparatedText = SplitStringOnDelimiter(textToDraw, '\n');
	//Strings lineSeparatedText = SplitStringOnDelimiter(text, '\n');
	int numLines = (int)lineSeparatedText.size();
	for (int lineIndex = 0; lineIndex < numLines; lineIndex++)
	{
		std::string line = lineSeparatedText[lineIndex];

		float lineWidth = GetTextWidth(cellHeight, line, cellAspect);
		//float lineHeight = cellHeight; // change for multi line text

		Vec2 lineMins;
		lineMins.x = box.m_mins.x + ((boxWidth - lineWidth) * alignment.x);
		lineMins.y = box.m_mins.y + ((boxHeight - textHeight) * alignment.y);
		lineMins.y += textHeight - (cellHeight * (float)lineIndex) - cellHeight;

		AddVertsForText2D(vertexArray, lineMins, cellHeight, line, tint);
	}
}

float BitmapFont::GetTextHeight(float cellHeight, std::string const& text, float cellAspect)
{
	UNUSED(cellAspect);

	Strings lineSeparatedText = SplitStringOnDelimiter(text, '\n');
	int numLines = (int)lineSeparatedText.size();

	float textHeight = numLines * cellHeight;
	return textHeight;
}


void BitmapFont::AddVertsForText3D(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, int maxGlyphToDraw)
{
	UNUSED(textMins);
	std::vector<Vertex_PCU> textVerts;

	// 1. create the text lying on xy plane
	std::string textToDraw = text;
	if (text.size() > maxGlyphToDraw)
	{
		textToDraw = text.substr(0, maxGlyphToDraw);
	}
	// text are in xy plane
	Strings textLines = SplitStringOnDelimiter(text, '\n');
	for (int lineNumber = 0; lineNumber < textLines.size(); lineNumber++)
	{
		std::string line = textLines[lineNumber];

		Vec2 lineMins = Vec2(0.f, 0.f);
		//float lineWidth = GetTextWidth(cellHeight, line, cellAspect);
		//lineMins.x = lineWidth + -1.f * alignment.x * 0.5f;
		lineMins.y = -1.f * lineNumber * cellHeight * cellAspect;


		AddVertsForText2D(textVerts, lineMins, cellHeight, line, tint, cellAspect);
	}
	//AddVertsForText2D(textVerts, Vec2::ZERO, cellHeight, text, tint, cellAspect);

	// 2. Transform the basis such that the text faces the i-axis (iForward)
	Mat44 identity;
	Mat44 iFacingTransform(Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
	//TransformVertexArray3D(textVerts, iFacingTransform);

	// 3. determine the 2d bounds of the vertex
	AABB2 textBounds = GetVertexBounds2D(textVerts);
	float shiftX = textBounds.GetDimensions().x * (alignment.x);
	float shiftY = textBounds.GetDimensions().y * (alignment.y);
	Vec3 shift(- shiftX, - shiftY, 0.f);
	shift = iFacingTransform.TransformPosition3D(shift);
	iFacingTransform.SetTranslation3D(shift);

	TransformVertexArray3D(textVerts, iFacingTransform); 


	// 4. append the verts
	verts.insert(verts.end(), textVerts.begin(), textVerts.end());
}
