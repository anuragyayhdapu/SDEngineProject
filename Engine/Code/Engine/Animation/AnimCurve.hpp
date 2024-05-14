#pragma once


#include "Engine/Animation/AnimKeyFrame.hpp"
#include "Engine/Animation/AnimUtils.hpp"

#include <vector>





//----------------------------------------------------------------------------------------------------------
// Frames are the smallest units, representing data at a specific time.
// Animation Curves are combination of multiple frames that describe how data changes over time.
//----------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
class FloatAnimCurve
{
public:
	std::vector<FloatKeyframe> m_keyframes;
	AnimInterpolationType	   m_interpolationType = AnimInterpolationType::Linear;
	AnimPlaybackType		   m_playbackType	   = AnimPlaybackType::ONCE;

	float		 Sample( float sampleTimeMilliSeconds, bool isLooping ) const;
	bool		 IsEmpty() const { return m_keyframes.empty(); }
	unsigned int GetSize() const { return ( unsigned int ) m_keyframes.size(); }

	float GetEndTimeMilliSeconds() const;
};


//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
struct Vec3AnimCurve
{
public:
	std::vector<Vector3Keyframe> m_keyframes;
	AnimInterpolationType		 m_interpolationType = AnimInterpolationType::Linear;
	AnimPlaybackType			 m_playbackType		 = AnimPlaybackType::ONCE;

	Vec3		 Sample( float sampleTimeMilliSeconds, bool isLooping ) const;
	bool		 IsEmpty() const { return m_keyframes.empty(); }
	unsigned int GetSize() const { return ( unsigned int ) m_keyframes.size(); }

	float GetEndTimeMilliSeconds() const;

	//----------------------------------------------------------------------------------------------------------
	void GetMaxAndMinZValuesFromCurve( float& zMin, float& zMax );
	void GetMaxAndMinXValuesFromCurve( float& xMin, float& xMax );

	//----------------------------------------------------------------------------------------------------------
	Vector3Keyframe const& GetKeyframeAtIndex( unsigned int index ) const;
	Vector3Keyframe const& GetKeyframeAtFirstIndex() const;
	Vector3Keyframe const& GetKeyframeAtLastIndex() const;
};


//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
struct QuaternionAnimCurve
{
public:
	std::vector<QuaternionKeyframe> m_keyframes;
	AnimInterpolationType			m_interpolationType = AnimInterpolationType::Linear;
	AnimPlaybackType				m_playbackType		= AnimPlaybackType::ONCE;

	Quaternion	 Sample( float sampleTimeMilliSeconds, bool isLooping ) const;
	bool		 IsEmpty() const { return m_keyframes.empty(); }
	unsigned int GetSize() const { return unsigned int( m_keyframes.size() ); }

	float GetEndTimeMilliSeconds() const;
};