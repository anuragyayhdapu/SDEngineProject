#pragma once

#include "Engine/Animation/AnimClip.hpp"
#include "Engine/Animation/AnimPose.hpp"

class AnimClip;

//----------------------------------------------------------------------------------------------------------
// Helper struct to store the information about the animation being faded to
struct AnimCrossFadeTarget
{
	AnimCrossFadeTarget() = default;

	AnimPose  m_sampledPose;
	AnimClip* m_targetAnimation				  = nullptr;
	float	  m_playbackTimeMilliseconds	  = 0.f;
	float	  m_fadeDurationMilliseconds	  = 0.f;
	float	  m_elapsedTimeInFadeMilliseconds = 0.f;
};



//----------------------------------------------------------------------------------------------------------
// Manages the playback of Animation and handles fading between them
class AnimCrossfadeController
{
public:
	AnimCrossfadeController();

	void Update( float deltaSeconds );

	// void AddNewFadingTarget( AnimClip* nextAnimClip, AnimPose const& defaultPose, float fadeDurationMilliSeconds );

	std::vector<AnimCrossFadeTarget> m_targets;

	AnimClip* m_currentAnimation				= nullptr;
	float	  m_currentPlaybackTimeMilliseconds = 0.f;
	AnimPose  m_currentSampledPose;


	//----------------------------------------------------------------------------------------------------------
};