#pragma once

#include "Engine/Animation/AnimChannel.hpp"

#include <vector>
#include <string>
#include <map>


class AnimPose;



//----------------------------------------------------------------------------------------------------------
// A snippet of Animation that can be played back.
// A character's walking cycle can be one clip, and their jump animation can be another.
class AnimClip
{
public:
	AnimClip() = default;

	std::string				 m_name = "UNKOWN ANIMATION CLIP";
	std::vector<AnimChannel> m_animChannels;
	bool					 m_isLooping = false;

	float		 Sample( float sampleTimeMilliSeconds, AnimPose& outPose ) const;
	AnimChannel& CreateOrGetAnimChannel( int jointId );

	float GetStartTime() const;
	float GetEndTime() const;

	void CalculateTimeStamps();

protected:
	float m_startTimeMilliseconds = 0.f;
	float m_endTimeMilliseconds	  = 0.f;

public:
	static AnimClip*						LoadOrGetAnimationClip( std::string clipFilePath );
	static std::map<std::string, AnimClip*> s_animClipRegistery;

	//----------------------------------------------------------------------------------------------------------
	bool				 m_removeRootMotion = false;
	Vec3AnimCurve&		 GetRootJointTranslationCurveByRefernce();
	Vec3AnimCurve const& GetRootJointTranslationCurve();
};