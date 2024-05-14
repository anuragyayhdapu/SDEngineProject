#pragma once


#include "Engine/Animation/AnimCurve.hpp"
#include "Engine/Math/Transform.hpp"

#include <string>


//----------------------------------------------------------------------------------------------------------
// Every joint has an animation channel, which stores the Animation curve of the joint's position, rotation, and scale.
class AnimChannel
{
public:
	AnimChannel() = default;

	Transform Sample( Transform const& reference, float sampleTimeMilliSeconds, bool isLooping, bool dontSampleTranslation = false ) const;
	bool	  IsValid() const;

	int			m_jointId	= -1;
	std::string m_jointName = "";

	Vec3AnimCurve		m_positionCurve;
	QuaternionAnimCurve m_rotationCurve;
	Vec3AnimCurve		m_scaleCurve;

	AnimPlaybackType m_playbackType = AnimPlaybackType::ONCE;

	float GetEndTimeMilliSeconds() const;
};