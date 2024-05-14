#pragma once

#include "Engine/Math/ConvexHull3.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"



//----------------------------------------------------------------------------------------------------------
// 2D
//----------------------------------------------------------------------------------------------------------
struct RaycastResult2D
{
	// Basic raycast result information (required)
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec2	m_impactPos;
	Vec2	m_impactNormal;

	// Original raycast information (optional)
	Vec2	m_rayFwdNormal;
	Vec2	m_rayStartPos;
	float	m_rayMaxLength = 1.f;
};


//----------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius);
RaycastResult2D RaycastVsLineSegment2D(Vec2 rayStartPos, Vec2 rayFwdNormal, float rayMaxDist, Vec2 lineSegmentStartPos, Vec2 lineSegmentEndPos);
RaycastResult2D RaycastVsAABB2D(Vec2 const& rayStartPos, Vec2 const& rayFwdNormal, float rayMaxDist, AABB2 const& bounds);


//----------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsPlane2D( Vec2 const& rayStartPos, Vec2 const& rayFwdNormal, float rayMaxDistance, Vec2 const& planeNormal, float planeDistanceFromOrigin );
RaycastResult2D RaycastVsConvexHull2D( Vec2 const& rayStartPos, Vec2 const& rayFwdNormal, float rayMaxDistance, ConvexHull2 const& convexHull, Vec2 const& boundingDiscCenter, float boundingDiscradius, bool useNarrowPhaseDiscOptimization = true );



//----------------------------------------------------------------------------------------------------------
// 3D
//----------------------------------------------------------------------------------------------------------
struct RaycastResult3D
{
	// Basic raycast result information (required)
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec3	m_impactPos;
	Vec3	m_impactNormal;

	// Original raycast information (optional)
	Vec3	m_rayFwdNormal;
	Vec3	m_rayStartPos;
	float	m_rayMaxLength = 1.f;
};


//----------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsCylinderZ3D(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDist, Vec2 const& center, float minZ, float maxZ, float radius);


//----------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsPlane3D( Vec3 const& rayStartPos, Vec3 const& rayFwdNormal, float rayMaxDistance, Vec3 const& planeNormal, float planeDistanceFromOrigin );
RaycastResult3D RaycastVsConvexHull3D( Vec3 const& rayStartPos, Vec3 const& rayFwdNormal, float rayMaxDistance, ConvexHull3 const& convexHull );