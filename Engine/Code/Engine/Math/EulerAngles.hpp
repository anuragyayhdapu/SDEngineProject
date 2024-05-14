#pragma once

#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"

struct EulerAngles
{
public:
	EulerAngles() = default;
	EulerAngles( float yawDegrees, float pitchDegrees, float rollDegrees);

	Vec3 GetVector_XFwd() const;
	void GetAsVectors_XFwd_YLeft_ZUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const;
	Mat44 GetAsMatrix_XFwd_YLeft_ZUp() const;

	void SetFromText(char const* text);

	bool ComponentWiseEquals( EulerAngles const& compare ) const;

	static EulerAngles const ZERO;

public:
	float m_yawDegrees		= 0.f;
	float m_pitchDegrees	= 0.f;
	float m_rollDegrees		= 0.f;
};