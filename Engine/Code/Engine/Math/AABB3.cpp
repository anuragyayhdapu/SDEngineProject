#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/EngineCommon.hpp"


AABB3::AABB3()
{
}

AABB3::AABB3(Vec3 mins, Vec3 maxs) :
	m_mins(mins), m_maxs(maxs)
{
}

AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) :
	m_mins(Vec3(minX, minY, minZ)), m_maxs(Vec3(maxX, maxY, maxZ))
{
}

void AABB3::Translate(Vec3 translation)
{
	m_mins += translation;
	m_maxs += translation;
}

const Vec3 AABB3::GetCenter() const
{
	return (m_mins + m_maxs) * 0.5F;
}

void AABB3::SetCenter(const Vec3& newCenter)
{
	Vec3 oldCenter = GetCenter();
	Vec3 deltaPosition = newCenter - oldCenter;

	Translate(deltaPosition);
}
