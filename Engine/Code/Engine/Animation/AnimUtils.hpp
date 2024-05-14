#pragma once


#include <vector>

struct FloatKeyframe;
struct Vector3Keyframe;
struct QuaternionKeyframe;


//------------------------------------------------------------------------------------------------
enum class AnimPlaybackType
{
	ONCE,	  // for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4...
	LOOP,	  // for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0...
	PINGPONG, // for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1...
};


//----------------------------------------------------------------------------------------------------------
enum class AnimInterpolationType
{
	Linear,
	Cubic
};


//----------------------------------------------------------------------------------------------------------
float AdjustSampleTimeToFitInsideRange( float sampleTimeMilliSeconds, float startTimeMs, float endTimeMs, AnimPlaybackType playbackType );


//----------------------------------------------------------------------------------------------------------
int GetKeyframeIndexAtSampleTime( std::vector<FloatKeyframe> const& m_keyframes, float sampleTime );
int GetKeyframeIndexAtSampleTime( std::vector<Vector3Keyframe> const& m_keyframes, float sampleTime );
int GetKeyframeIndexAtSampleTime( std::vector<QuaternionKeyframe> const& m_keyframes, float sampleTime );
