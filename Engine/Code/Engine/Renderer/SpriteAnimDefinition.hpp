#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"

//------------------------------------------------------------------------------------------------
enum class SpriteAnimPlaybackType
{
	ONCE,		// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4...
	LOOP,		// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0...
	PINGPONG,	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1...
};


//------------------------------------------------------------------------------------------------
class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition(SpriteSheet const& sheet, int startSpriteIndex = -1, int endSpriteIndex = -1,
		float framesPerSecond = 20.f, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::ONCE);

	void LoadFromNamedStrings(NamedStrings const& spriteDefNamedStrings );

	SpriteDefinition const& GetSpriteDefAtTime(float seconds) const;

	std::string m_name = "UNKOWN SPRITE ANIMATION";

	float GetSecondsPerFrame() const {return m_secondsPerFrame; }
	int GetTotalSpriteIndexes() const { return (m_endSpriteIndex - m_startSpriteIndex) + 1; }

private:

	SpriteSheet	const&		m_spriteSheet;
	int						m_startSpriteIndex	= -1;
	int						m_endSpriteIndex	= -1;
	float					m_secondsPerFrame	= 0.05f; // Client specifies FPS in constructor, but we store period = 1/FPS
	SpriteAnimPlaybackType	m_playbackType		= SpriteAnimPlaybackType::ONCE;
};
