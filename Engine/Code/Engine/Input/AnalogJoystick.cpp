#include "AnalogJoystick.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <Engine/Math/MathUtils.hpp>
#include <math.h>
#include <Engine/Core/ErrorWarningAssert.hpp>

Vec2 AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}

float AnalogJoystick::GetMagnitude() const
{
	return m_correctedPosition.GetLength();
}

float AnalogJoystick::GetOrientationDegrees() const
{
	return m_correctedPosition.GetOrientationDegrees();
}

Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}

float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}

float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}

void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2();
	m_correctedPosition = Vec2();
}

void AnalogJoystick::SetDeadZoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold)
{
	m_innerDeadZoneFraction = normalizedInnerDeadzoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadzoneThreshold;
}


void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{
	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);

	// convert to polar coordinates
	float R = m_rawPosition.GetLength();
	float thetaDegrees = m_rawPosition.GetOrientationDegrees();

	// check if R is within Range
	// range-map clamp R
	float correctedR = RangeMapClamped(R, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f);

	// get x and y from r and theta
	m_correctedPosition = Vec2::MakeFromPolarDegrees(thetaDegrees, correctedR);

	//DebuggerPrintf("%f\n", GetMagnitude());
}
