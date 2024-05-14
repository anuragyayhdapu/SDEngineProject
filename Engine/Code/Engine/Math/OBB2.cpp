#include "OBB2.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"

OBB2::OBB2()
{
}

OBB2::OBB2(Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const& halfDimensions) :
	m_center(center), m_iBasisNormal(iBasisNormal), m_halfDimensions(halfDimensions)
{
}

OBB2::OBB2(Vec2 const& center, float orientationDegrees, Vec2 const& dimensions)
{
	m_center = center;

	// get ibasis normal from orientation
	m_iBasisNormal = Vec2::MakeFromPolarDegrees(orientationDegrees);

	m_halfDimensions = dimensions * 0.5f;
}

bool OBB2::IsPointInside(Vec2 const& referncePoint) const
{
	bool isPointinside = IsPointInsideOBB2D(referncePoint, *this);
	return isPointinside;
}

Vec2 const OBB2::GetNearestPoint(Vec2 const& referncePoint) const
{
	Vec2 nearestPoint = GetNearestPointOnOBB2D(referncePoint, *this);
	return nearestPoint;
}
