#pragma once

#include "Engine/Math/Vec2.hpp"


// owns data and logic for joystick positioning & dead-zone corrections
class AnalogJoystick
{
public:
	Vec2 GetPosition() const;
	Vec2 GetRawUncorrectedPosition() const;
	float GetInnerDeadZoneFraction() const;
	float GetOuterDeadZoneFraction() const;

	float GetMagnitude() const;
	float GetOrientationDegrees() const;

	// For use by Xbox Controller
	void Reset();
	void SetDeadZoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold);
	void UpdatePosition(float rawNormalizedX, float rawNormalizedY);

protected:
	Vec2 m_rawPosition;						// Flaky, doesn't reset to zero (or consistently snap to rest position)
	Vec2 m_correctedPosition;				// Deadzone-corrected position
	float m_innerDeadZoneFraction = 0.00f;	// if R < this%, R = 0; "input range start" for corrective range map
	float m_outerDeadZoneFraction = 1.00f;	// if R > this%, R = 1; "input range end" for corrective range map
};