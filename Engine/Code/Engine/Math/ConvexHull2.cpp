#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Math/MathUtils.hpp"



//----------------------------------------------------------------------------------------------------------
ConvexHull2::ConvexHull2()
{
}


//----------------------------------------------------------------------------------------------------------
ConvexHull2::ConvexHull2( ConvexPolly2 const& convexPolly2 )
{
	// 1. get normal of each line segment of the poly2
	std::vector<Vec2> polyPoints = convexPolly2.GetCounterClockWiseOrderedPoints();

	// 0, 1
	if (polyPoints.size() <= 1)
	{
		return;
	}

	// 2
	if (polyPoints.size() <= 2)
	{
		Vec2 polyPoint1 = polyPoints[ 0 ];
		Vec2 polyPoint2 = polyPoints[ 1 ];

		Vec2 polySideVector = polyPoint2 - polyPoint1;
		Vec2 planeNormal	= polySideVector.GetRotatedMinus90Degrees();
		planeNormal.Normalize();

		float planeDistanceFromOrigin = DotProduct2D( polyPoint1, planeNormal );

		Plane2 plane;
		plane.m_normal			   = planeNormal;
		plane.m_distanceFromOrigin = planeDistanceFromOrigin;

		m_boundingPlanes.push_back( plane );

		return;
	}

	// > 3
	for (int index = 0; index < polyPoints.size() - 1; index++)
	{
		Vec2 polyPoint1 = polyPoints[ index ];
		Vec2 polyPoint2 = polyPoints[ index + 1 ];

		Vec2 polySideVector = polyPoint2 - polyPoint1;
		Vec2 planeNormal	= polySideVector.GetRotatedMinus90Degrees();
		planeNormal.Normalize();

		float planeDistanceFromOrigin = DotProduct2D( polyPoint1, planeNormal );
		
		Plane2 plane;
		plane.m_normal = planeNormal;
		plane.m_distanceFromOrigin = planeDistanceFromOrigin;

		m_boundingPlanes.push_back( plane );
	}

	// plane from last point to first point
	Vec2 lastPolyPoint = polyPoints[ polyPoints.size() - 1 ];
	Vec2 firstPolypoint = polyPoints[ 0 ];

	Vec2 polySideVector = firstPolypoint - lastPolyPoint;
	Vec2 planeNormal	= polySideVector.GetRotatedMinus90Degrees();
	planeNormal.Normalize();

	float planeDistanceFromOrigin = DotProduct2D( lastPolyPoint, planeNormal );

	Plane2 plane;
	plane.m_normal			   = planeNormal;
	plane.m_distanceFromOrigin = planeDistanceFromOrigin;

	m_boundingPlanes.push_back( plane );
}
