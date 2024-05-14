#pragma once

#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vec3.hpp"


//----------------------------------------------------------------------------------------------------------
struct FloatKeyframe
{
	float m_timeMilliSeconds;
	float m_value;
};


//----------------------------------------------------------------------------------------------------------
struct Vector3Keyframe
{
	float m_timeMilliSeconds;
	Vec3  m_value;
};


//----------------------------------------------------------------------------------------------------------
struct QuaternionKeyframe
{
	float	   m_timeMilliSeconds;
	Quaternion m_value;
};