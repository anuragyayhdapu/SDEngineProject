#pragma once


#include "Engine/Math/Vec3.hpp"


//----------------------------------------------------------------------------------------------------------
struct Plane3
{
	explicit Plane3();
	explicit Plane3(Vec3 const& normal, float distanceFromOrigin);

	Vec3  m_normal			   = Vec3::ZERO;
	float m_distanceFromOrigin = 0.f;

	static const Plane3 MakeFromVectorsInPlane(Vec3 const& A, Vec3 const& B);
};