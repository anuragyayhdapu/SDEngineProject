#include "Engine/Math/ConvexHull3.hpp"
#include "Engine/Math/MathUtils.hpp"



//----------------------------------------------------------------------------------------------------------
ConvexHull3::ConvexHull3()
{
}


//----------------------------------------------------------------------------------------------------------
ConvexHull3::ConvexHull3( AABB3 const& bounds )
{
	// calculate the planes from each side of the AABB3

	// face 1 : -x plane : front
	Vec3  normalFront		   = Vec3( -1.f, 0.f, 0.f );
	float distanceToFrontPlane = DotProduct3D( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), normalFront );
	m_boundingPlanes.push_back( Plane3( normalFront, distanceToFrontPlane ) );

	// face 3 : +x plane : back
	Vec3  normalBack					= -1.f * normalFront;
	float distanceFromOriginToBackPlane = DotProduct3D( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), normalBack );
	m_boundingPlanes.push_back( Plane3( normalBack, distanceFromOriginToBackPlane ) );

	// face 4 : +y plane : west
	Vec3  normalWest		  = Vec3( 0.f, 1.f, 0.f );
	float distanceToWestPlane = DotProduct3D( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), normalWest );
	m_boundingPlanes.push_back( Plane3( normalWest, distanceToWestPlane ) );

	// face 2 : -y_plane : east
	Vec3  normalEast		  = Vec3( 0.f, -1.f, 0.f );
	float distanceToEastPlane = DotProduct3D( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), normalEast );
	m_boundingPlanes.push_back( Plane3( normalEast, distanceToEastPlane ) );

	// face 6 : +z plane : top
	Vec3  normalTop			 = Vec3( 0.f, 0.f, 1.f );
	float distanceToTopPlane = DotProduct3D( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), normalTop );
	m_boundingPlanes.push_back( Plane3( normalTop, distanceToTopPlane ) );

	// face 5 : -z_plane : bottom
	Vec3  normalBottom			= Vec3( 0.f, 0.f, -1.f );
	float distanceToBottomPlane = DotProduct3D( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), normalBottom );
	m_boundingPlanes.push_back( Plane3( normalBottom, distanceToBottomPlane ) );
}
