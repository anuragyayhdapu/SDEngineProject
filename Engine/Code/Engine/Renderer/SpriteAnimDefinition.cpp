#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet const& sheet, int startSpriteIndex,
	int endSpriteIndex, float framesPerSecond, SpriteAnimPlaybackType playbackType)
	: m_spriteSheet(sheet), m_startSpriteIndex(startSpriteIndex), m_endSpriteIndex(endSpriteIndex),
	m_secondsPerFrame(1.f / framesPerSecond), m_playbackType(playbackType)
{
}


void SpriteAnimDefinition::LoadFromNamedStrings(NamedStrings const& spriteDefNamedStrings)
{
	m_name = spriteDefNamedStrings.GetValue("name", "UNNAMED SPRITE ANIMATION");
	
	m_startSpriteIndex = spriteDefNamedStrings.GetValue("startFrame", m_startSpriteIndex);
	m_endSpriteIndex = spriteDefNamedStrings.GetValue("endFrame", m_endSpriteIndex);
	m_secondsPerFrame = spriteDefNamedStrings.GetValue("secondsPerFrame", m_secondsPerFrame);

	std::string playbackTypeStr = spriteDefNamedStrings.GetValue("playbackMode", "UNKOWN PLAYBACK TYPE");
	if ( playbackTypeStr == "Once" )
	{
		m_playbackType = SpriteAnimPlaybackType::ONCE;
	}
	else if ( playbackTypeStr == "Loop" )
	{
		m_playbackType = SpriteAnimPlaybackType::LOOP;
	}
	else if ( playbackTypeStr == "PingPong" )
	{
		m_playbackType = SpriteAnimPlaybackType::PINGPONG;
	}
}



SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{
	// based on the given time and spf, get the correct sprite index

	if (m_playbackType == SpriteAnimPlaybackType::LOOP)
	{
		int frameIndex = (int)(seconds / m_secondsPerFrame); // seconds = 1.3, m_secondsPerFrame = 0.1, frameIndex = 13

		int numFramesInAnimation = m_endSpriteIndex - m_startSpriteIndex + 1; // m_endSpriteIndex = 6, m_startSpriteIndex = 3, numFramesInAnimation = 4
		int animIndex = frameIndex % (numFramesInAnimation); // frameIndex = 13, numFramesInAnimation = 4, animIndex = 1
		int spriteIndex = m_startSpriteIndex + animIndex; // m_startSpriteIndex = 3, frameIndex = 1, spriteIndex = 4

		return m_spriteSheet.GetSpriteDef(spriteIndex);
	}
	else if (m_playbackType == SpriteAnimPlaybackType::PINGPONG)
	{
		int frameIndex = (int)(seconds / m_secondsPerFrame);
		int numFramesInAnimation = (m_endSpriteIndex - m_startSpriteIndex) * 2;
		int animIndex = frameIndex % (numFramesInAnimation);

		int numForwardFramesInAnimation = m_endSpriteIndex - m_startSpriteIndex + 1;
		if (animIndex >= numForwardFramesInAnimation) // reverse frames
		{
			animIndex = numFramesInAnimation - animIndex;
		}

		int spriteIndex = m_startSpriteIndex + animIndex;


		return m_spriteSheet.GetSpriteDef(spriteIndex);
	}
	else // ONCE
	{
		int frameIndex = (int)(seconds / m_secondsPerFrame); // e.g. seconds = 0.34, m_secondsPerFrame = 0.1, frameIndex = 3
		int spriteIndex = m_startSpriteIndex + frameIndex; // m_startSpriteIndex = 23, frameIndex = 3, spriteIndex = 26

		if (spriteIndex < m_startSpriteIndex)
		{
			spriteIndex = m_startSpriteIndex;
		}
		else if (spriteIndex > m_endSpriteIndex)
		{
			spriteIndex = m_endSpriteIndex;
		}

		return m_spriteSheet.GetSpriteDef(spriteIndex);
	}
}
