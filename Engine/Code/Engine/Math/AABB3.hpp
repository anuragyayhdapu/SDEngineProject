#pragma once

#include "Engine/Math/Vec3.hpp"

class AABB3
{
public:
	Vec3 m_mins = Vec3(0.f, 0.f, 0.f);
	Vec3 m_maxs = Vec3(0.f, 0.f, 0.f);

	AABB3();
	AABB3(Vec3 mins, Vec3 maxs);
	AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

	void Translate(Vec3 translation);

	const Vec3 GetCenter() const;
	void SetCenter(const Vec3& newCenter);
};