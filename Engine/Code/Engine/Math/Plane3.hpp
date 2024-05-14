#pragma once


#include "Engine/Math/Vec3.hpp"


//----------------------------------------------------------------------------------------------------------
struct Plane3
{
	explicit Plane3();
	explicit Plane3(Vec3 const& normal, float distanceFromOrigin);

	Vec3  m_normal;
	float m_distanceFromOrigin;

	static const Plane3 MakeFromVectorsInPlane(Vec3 const& A, Vec3 const& B);
};