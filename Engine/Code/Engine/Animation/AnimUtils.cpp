#include "Engine/Animation/AnimUtils.hpp"
#include "Engine/Animation/AnimCurve.hpp"

#include <math.h>



//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
float AdjustSampleTimeToFitInsideRange( float sampleTimeMilliSeconds, float startTimeMs, float endTimeMs, AnimPlaybackType playbackType )
{
	float adjustedSampleTime = sampleTimeMilliSeconds;

	if ( playbackType == AnimPlaybackType::ONCE )
	{
		if ( sampleTimeMilliSeconds < startTimeMs )
		{
			adjustedSampleTime = startTimeMs;
		}
		else if ( sampleTimeMilliSeconds > endTimeMs )
		{
			adjustedSampleTime = endTimeMs;
		}
	}
	else if ( playbackType == AnimPlaybackType::LOOP )
	{
		if ( sampleTimeMilliSeconds < startTimeMs )
		{
			adjustedSampleTime = startTimeMs;
		}
		else if ( sampleTimeMilliSeconds > endTimeMs )
		{
			float curveDuration = endTimeMs - startTimeMs;
			adjustedSampleTime	= startTimeMs + fmodf( sampleTimeMilliSeconds - startTimeMs, curveDuration );
		}
	}
	else // AnimPlaybackType::Ping Pong
	{
		// TODO: later
	}

	return adjustedSampleTime;
}



//----------------------------------------------------------------------------------------------------------
int GetKeyframeIndexAtSampleTime( std::vector<FloatKeyframe> const& m_keyframes, float sampleTimeMilliSeconds )
{
	int numFrames = ( int ) m_keyframes.size();
	if ( numFrames <= 1 )
	{
		return -1;
	}

	float timeOfFirstFrame		= m_keyframes[ 0 ].m_timeMilliSeconds;
	int	  secondLastIndex		= ( int ) m_keyframes.size() - 2;
	float timeOfSecondLastFrame = m_keyframes[ secondLastIndex ].m_timeMilliSeconds;


	if ( sampleTimeMilliSeconds <= timeOfFirstFrame )
	{
		return 0;
	}
	else if ( sampleTimeMilliSeconds >= timeOfSecondLastFrame )
	{
		return secondLastIndex;
	}
	else
	{
		// loop through the frames to find the correct frame index at given time
		for ( int index = 0; index < m_keyframes.size(); index++ )
		{
			float timeAtThisFrame = m_keyframes[ index ].m_timeMilliSeconds;
			if ( timeAtThisFrame >= sampleTimeMilliSeconds )
			{
				return index - 1;
			}
		}
	}

	return -1;
}


//----------------------------------------------------------------------------------------------------------
int GetKeyframeIndexAtSampleTime( std::vector<Vector3Keyframe> const& m_keyframes, float sampleTimeMilliSeconds )
{
	int numFrames = ( int ) m_keyframes.size();
	if ( numFrames <= 1 )
	{
		return -1;
	}

	float timeOfFirstFrame		= m_keyframes[ 0 ].m_timeMilliSeconds;
	int	  secondLastIndex		= ( int ) m_keyframes.size() - 2;
	float timeOfSecondLastFrame = m_keyframes[ secondLastIndex ].m_timeMilliSeconds;


	if ( sampleTimeMilliSeconds <= timeOfFirstFrame )
	{
		return 0;
	}
	else if ( sampleTimeMilliSeconds >= timeOfSecondLastFrame )
	{
		return secondLastIndex;
	}
	else
	{
		// loop through the frames to find the correct frame index at given time
		for ( int index = 0; index < m_keyframes.size(); index++ )
		{
			float timeAtThisFrame = m_keyframes[ index ].m_timeMilliSeconds;
			if ( timeAtThisFrame >= sampleTimeMilliSeconds )
			{
				return index - 1;
			}
		}
	}

	return -1;
}


//----------------------------------------------------------------------------------------------------------
int GetKeyframeIndexAtSampleTime( std::vector<QuaternionKeyframe> const& m_keyframes, float sampleTimeMilliSeconds )
{
	int numFrames = ( int ) m_keyframes.size();
	if ( numFrames <= 1 )
	{
		return -1;
	}

	float timeOfFirstFrame		= m_keyframes[ 0 ].m_timeMilliSeconds;
	int	  secondLastIndex		= ( int ) m_keyframes.size() - 2;
	float timeOfSecondLastFrame = m_keyframes[ secondLastIndex ].m_timeMilliSeconds;


	if ( sampleTimeMilliSeconds <= timeOfFirstFrame )
	{
		return 0;
	}
	else if ( sampleTimeMilliSeconds >= timeOfSecondLastFrame )
	{
		return secondLastIndex;
	}
	else
	{
		// loop through the frames to find the correct frame index at given time
		for ( int index = 0; index < m_keyframes.size(); index++ )
		{
			float timeAtThisFrame = m_keyframes[ index ].m_timeMilliSeconds;
			if ( timeAtThisFrame >= sampleTimeMilliSeconds )
			{
				return index - 1;
			}
		}
	}

	return -1;
}