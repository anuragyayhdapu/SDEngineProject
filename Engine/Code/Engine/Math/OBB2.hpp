#pragma once

#include "Engine/Math/Vec2.hpp"

class OBB2
{
public:
	OBB2();
	OBB2(Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions);
	OBB2(Vec2 const& center, float orientationDegrees, Vec2 const& dimensions);

	Vec2 m_center;
	Vec2 m_iBasisNormal = Vec2(1.0f, 0.0f);
	Vec2 m_halfDimensions;

	bool IsPointInside(Vec2 const& referncePoint) const;
	Vec2 const GetNearestPoint(Vec2 const& referncePoint) const;
};