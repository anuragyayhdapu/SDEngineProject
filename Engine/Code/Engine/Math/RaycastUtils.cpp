#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


#include <map>


RaycastResult2D RaycastVsDisc2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius )
{
	RaycastResult2D result;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayStartPos  = startPos;
	result.m_rayMaxLength = maxDist;

	// 1. Compute ray left normal
	Vec2 rayiBasis = fwdNormal;
	Vec2 rayjBasis = Vec2( -rayiBasis.y, rayiBasis.x );

	// 2. compute displacement from ray start position to disc center
	Vec2 displacementSC = discCenter - startPos;

	// 3. Compute Dot of SC and left normal
	float SCj = DotProduct2D( displacementSC, rayjBasis );

	// 4. Check if disc is too far to the left or right
	if ( SCj >= discRadius || SCj <= -discRadius )
	{
		result.m_didImpact = false;
		return result;
	}

	// 5. Compute Dot of SC and forward normal
	float SCi = DotProduct2D( displacementSC, rayiBasis );
	/*DebuggerPrintf("SCi: %f, -discRadius: %f,  maxDist + discRadius: %f", SCi, -discRadius, maxDist + discRadius);
	DebuggerPrintf("---\n");*/

	// 6. Check if disc is entirely before or after the ray
	if ( SCi <= -discRadius || SCi >= maxDist + discRadius )
	{
		result.m_didImpact = false;
		return result;
	}

	// 7. Check if raycast start is inside the circle
	if ( IsPointInsideDisc2D( startPos, discCenter, discRadius ) )
	{
		result.m_didImpact	  = true;
		result.m_impactPos	  = startPos;
		result.m_impactDist	  = 0.f;
		result.m_impactNormal = -fwdNormal;
		return result;
	}

	// 8. Compute the portion of ray inside the circle
	float rayInsideCircle		= sqrtf( ( discRadius * discRadius ) - ( SCj * SCj ) );
	float raycastImpactDistance = SCi - rayInsideCircle;

	// 9. impact is before ray start or impact is after ray end
	if ( raycastImpactDistance <= 0.f || raycastImpactDistance >= maxDist )
	{
		result.m_didImpact = false;
		return result;
	}

	// 10. otherwise raycast hit
	// compute raycast impact position & impact normal
	result.m_didImpact	  = true;
	result.m_impactDist	  = raycastImpactDistance;
	result.m_impactPos	  = startPos + ( rayiBasis * result.m_impactDist );
	result.m_impactNormal = ( result.m_impactPos - discCenter ).GetNormalized();

	return result;
}

RaycastResult2D RaycastVsLineSegment2D( Vec2 rayStartPos, Vec2 rayFwdNormal, float rayMaxDist, Vec2 lineSegmentStartPos, Vec2 lineSegmentEndPos )
{
	RaycastResult2D result;
	result.m_rayStartPos  = rayStartPos;
	result.m_rayFwdNormal = rayFwdNormal;
	result.m_rayMaxLength = rayMaxDist;

	// 1. general case
	Vec2  i	  = rayFwdNormal;
	Vec2  j	  = rayFwdNormal.GetRotated90Degrees();
	Vec2  SA  = lineSegmentStartPos - rayStartPos;
	Vec2  SB  = lineSegmentEndPos - rayStartPos;
	float SAj = DotProduct2D( SA, j );
	float SBj = DotProduct2D( SB, j );

	// side ways miss
	if ( SAj <= 0.f && SBj <= 0.f )
	{
		return result; // miss, entirely right of ray
	}
	if ( SAj >= 0.f && SBj >= 0.f )
	{
		return result; // miss, entirely left of ray
	}

	float t = ( -1.f * SAj ) / ( SBj - SAj ); // the time at which the ray impacted the line segment

	Vec2 A		   = lineSegmentStartPos;
	Vec2 B		   = lineSegmentEndPos;
	Vec2 impactPos = A + ( t * ( B - A ) );

	// check which side is hit
	Vec2 lineSegment		  = lineSegmentEndPos - lineSegmentStartPos;
	Vec2 lineSegmentDirection = lineSegment.GetNormalized();
	Vec2 impactNormal		  = lineSegmentDirection.GetRotated90Degrees();

	float rayFwd_Dot_impactNormal = DotProduct2D( rayFwdNormal, impactNormal );
	if ( rayFwd_Dot_impactNormal > 0.f )
	{
		impactNormal *= -1.f; // calculate normal of back side, equivalent to GetRotated by 90 degrees
	}

	Vec2  impactRay		 = impactPos - rayStartPos;
	float impactDistance = DotProduct2D( impactRay, rayFwdNormal );
	if ( impactDistance >= rayMaxDist || impactDistance <= 0.f )
	{
		// miss
		return result;
	}


	// hit
	result.m_didImpact	  = true;
	result.m_impactNormal = impactNormal;
	result.m_impactPos	  = impactPos;
	result.m_impactDist	  = impactDistance;
	return result;
}

RaycastResult2D RaycastVsAABB2D( Vec2 const& rayStartPos, Vec2 const& rayFwdNormal, float rayMaxDist, AABB2 const& bounds )
{
	RaycastResult2D result;
	result.m_rayStartPos  = rayStartPos;
	result.m_rayFwdNormal = rayFwdNormal;
	result.m_rayMaxLength = rayMaxDist;

	// if inside the AABB2, automatic hit
	bool isPointInsideAABB2 = bounds.IsPointInside( rayStartPos );
	if ( isPointInsideAABB2 )
	{
		Vec2 impactPos = bounds.GetNearestPoint( rayStartPos );

		result.m_didImpact	  = true;
		result.m_impactNormal = rayFwdNormal * -1.f;
		result.m_impactPos	  = impactPos;
		result.m_impactDist	  = 0.f;
		return result;
	}


	// 1. calculate t with minX
	float tMinX = ( bounds.m_mins.x - rayStartPos.x ) / rayFwdNormal.x;

	// 2. calculate t with maxX
	float tMaxX = ( bounds.m_maxs.x - rayStartPos.x ) / rayFwdNormal.x;

	// 3. calculate t with minY
	float tMinY = ( bounds.m_mins.y - rayStartPos.y ) / rayFwdNormal.y;

	// 4. calculate t with maxY
	float tMaxY = ( bounds.m_maxs.y - rayStartPos.y ) / rayFwdNormal.y;

	// calculate float ranges
	FloatRange xFloatRange;
	if ( tMinX < tMaxX )
	{
		xFloatRange = FloatRange( tMinX, tMaxX );
	}
	else
	{
		xFloatRange = FloatRange( tMaxX, tMinX );
	}

	FloatRange yFloatRange;
	if ( tMinY < tMaxY )
	{
		yFloatRange = FloatRange( tMinY, tMaxY );
	}
	else
	{
		yFloatRange = FloatRange( tMaxY, tMinY );
	}

	if ( xFloatRange.IsOverlappingWith( yFloatRange ) )
	{
		// hit
		float impactTime = 0.f;

		// find the second smallest impact time
		if ( xFloatRange.GetMin() < yFloatRange.GetMin() )
		{
			impactTime = yFloatRange.GetMin();
		}
		else
		{
			impactTime = xFloatRange.GetMin();
		}

		if ( impactTime < 0.f )
		{
			// early miss
			result.m_didImpact = false;
			return result;
		}


		Vec2  impactPos		 = rayStartPos + ( rayFwdNormal * impactTime );
		Vec2  impactVector	 = impactPos - rayStartPos;
		float impactDistance = impactVector.GetLength();

		Vec2 impactNormal = Vec2::ZERO;
		if ( impactPos.x == bounds.m_mins.x )
		{
			impactNormal = Vec2( -1.f, 0.f );
		}
		else if ( impactPos.x == bounds.m_maxs.x )
		{
			impactNormal = Vec2( 1.f, 0.f );
		}
		else if ( impactPos.y == bounds.m_mins.y )
		{
			impactNormal = Vec2( 0.f, -1.f );
		}
		else if ( impactPos.y == bounds.m_maxs.y )
		{
			impactNormal = Vec2( 0.f, 1.f );
		}

		result.m_didImpact	  = true;
		result.m_impactNormal = impactNormal;
		result.m_impactPos	  = impactPos;
		result.m_impactDist	  = impactDistance;
		return result;
	}

	// miss
	result.m_didImpact = false;
	return result;
}


//----------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsPlane2D( Vec2 const& rayStartPos, Vec2 const& rayFwdNormal, float rayMaxDistance, Vec2 const& planeNormal, float planeDistanceFromOrigin )
{
	RaycastResult2D result;
	result.m_didImpact	  = false;
	result.m_rayStartPos  = rayStartPos;
	result.m_rayFwdNormal = rayFwdNormal;
	result.m_rayMaxLength = rayMaxDistance;

	// 1. get altitude of ray start position
	float startPointAltitude = DotProduct2D( rayStartPos, planeNormal );
	startPointAltitude		 = startPointAltitude - planeDistanceFromOrigin;

	// 2. get altitude of ray end position
	Vec2  rayEndPos		   = rayStartPos + ( rayFwdNormal * rayMaxDistance );
	float endPointAltitude = DotProduct2D( rayEndPos, planeNormal );
	endPointAltitude	   = endPointAltitude - planeDistanceFromOrigin;

	// 3. one should be positive and other should be negative
	if ( startPointAltitude * endPointAltitude >= 0.f )
	{
		return result;
	}

	result.m_didImpact = true;


	// 4. find the impact point
	float rayProjectedOnPlaneNormal = GetProjectedLength2D( rayFwdNormal, planeNormal );
	result.m_impactDist				= -1.f * ( startPointAltitude / rayProjectedOnPlaneNormal );

	if ( startPointAltitude < 0.f )
	{
		result.m_impactNormal = -planeNormal;
	}
	else
	{
		result.m_impactNormal = planeNormal;
	}

	result.m_impactPos = rayStartPos + ( rayFwdNormal * result.m_impactDist );


	return result;
}


// struct RaycastResult2DConvexHullExt : RaycastResult2D
//{
//	RaycastResult2DConvexHullExt() {}
//
//	int planeIndex = -1;
// };



//----------------------------------------------------------------------------------------------------------
bool IsPointInFrontOfPlane2D( Vec2 const& point, Plane2 const& plane )
{
	float pointAltitude = DotProduct2D( point, plane.m_normal );
	if ( pointAltitude > plane.m_distanceFromOrigin )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------
int GetRaycastResultFurthestFromStart2D( std::vector<RaycastResult2D> const& raycastList )
{
	int	  resultIndex = -1;
	float maxDistance = 0.f;

	for ( int index = 0; index < raycastList.size(); index++ )
	{
		float impactDistance = raycastList[ index ].m_impactDist;
		if ( impactDistance > maxDistance )
		{
			maxDistance = impactDistance;
			resultIndex = index;
		}
	}

	return resultIndex;
}


//----------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsConvexHull2D( Vec2 const& rayStartPos, Vec2 const& rayFwdNormal, float rayMaxDistance, ConvexHull2 const& convexHull, Vec2 const& boundingDiscCenter, float boundingDiscRadius, bool useNarrowPhaseDiscOptimization )
{
	RaycastResult2D finalResult;
	finalResult.m_rayStartPos  = rayStartPos;
	finalResult.m_rayFwdNormal = rayFwdNormal;
	finalResult.m_rayMaxLength = rayMaxDistance;
	finalResult.m_didImpact	   = false;

	if (useNarrowPhaseDiscOptimization)
	{
		// 1. check if start point is inside the convex plane
		if ( IsPointInsideConvexHull2D( rayStartPos, convexHull, boundingDiscCenter, boundingDiscRadius ) )
		{
			finalResult.m_didImpact	   = true;
			finalResult.m_impactPos	   = rayStartPos;
			finalResult.m_impactDist   = 0.f;
			finalResult.m_impactNormal = -1.f * rayFwdNormal;

			// hit
			return finalResult;
		}
	}


	// 2. check raycast with all planes
	std::vector<RaycastResult2D> entryRaycastResults;
	std::vector<RaycastResult2D> exitRaycastResults;
	for ( int index = 0; index < convexHull.m_boundingPlanes.size(); index++ )
	{
		Plane2 const&	plane  = convexHull.m_boundingPlanes[ index ];
		RaycastResult2D result = RaycastVsPlane2D( rayStartPos, rayFwdNormal, rayMaxDistance, plane.m_normal, plane.m_distanceFromOrigin );
		if ( result.m_didImpact )
		{
			// is entry or exit point
			bool isStartPointInFrontOfPlane = IsPointInFrontOfPlane2D( rayStartPos, plane );
			if ( isStartPointInFrontOfPlane ) // in front of plane
			{
				entryRaycastResults.push_back( result );
			}
			else // behind plane
			{
				exitRaycastResults.push_back( result );
			}
		}
	}

	// 3. check if no entry point, start is outside the hull
	if ( entryRaycastResults.empty() )
	{
		// miss
		return finalResult;
	}


	// 4. check if last entry point is inside the convex hull
	int				furthestIndex		   = GetRaycastResultFurthestFromStart2D( entryRaycastResults );
	RaycastResult2D lastEntryRaycastResult = entryRaycastResults[ furthestIndex ];
	Vec2			lastEntryImpactPos	   = lastEntryRaycastResult.m_impactPos;
	if ( IsPointInsideConvexHull2D( lastEntryImpactPos, convexHull, boundingDiscCenter, boundingDiscRadius ) )
	{
		// hit
		finalResult = lastEntryRaycastResult;
		return finalResult;
	}

	// miss
	return finalResult;
}


//----------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsCylinderZ3D( Vec3 const& startPos, Vec3 const& fwdNormal, float maxDist, Vec2 const& center, float minZ, float maxZ, float radius )
{
	UNUSED( startPos );
	UNUSED( fwdNormal );
	UNUSED( maxDist );
	UNUSED( center );
	UNUSED( minZ );
	UNUSED( maxZ );
	UNUSED( radius );

	return RaycastResult2D();
}


//----------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsPlane3D( Vec3 const& rayStartPos, Vec3 const& rayFwdNormal, float rayMaxDistance, Vec3 const& planeNormal, float planeDistanceFromOrigin )
{
	RaycastResult3D result;
	result.m_didImpact	  = false;
	result.m_rayStartPos  = rayStartPos;
	result.m_rayFwdNormal = rayFwdNormal;
	result.m_rayMaxLength = rayMaxDistance;

	// 1. get altitude of ray start position
	float startPointAltitude = DotProduct3D( rayStartPos, planeNormal );
	startPointAltitude		 = startPointAltitude - planeDistanceFromOrigin;

	// 2. get altitude of ray end position
	Vec3  rayEndPos		   = rayStartPos + ( rayFwdNormal * rayMaxDistance );
	float endPointAltitude = DotProduct3D( rayEndPos, planeNormal );
	endPointAltitude	   = endPointAltitude - planeDistanceFromOrigin;

	// 3. one should be positive and other should be negative
	if ( startPointAltitude * endPointAltitude >= 0.f )
	{
		return result;
	}

	result.m_didImpact = true;


	// 4. find the impact point
	float rayProjectedOnPlaneNormal = GetProjectedLength3D( rayFwdNormal, planeNormal );
	result.m_impactDist				= -1.f * ( startPointAltitude / rayProjectedOnPlaneNormal );

	if ( startPointAltitude < 0.f )
	{
		result.m_impactNormal = -1.f * planeNormal;
	}
	else
	{
		result.m_impactNormal = planeNormal;
	}

	result.m_impactPos = rayStartPos + ( rayFwdNormal * result.m_impactDist );


	return result;
}


//----------------------------------------------------------------------------------------------------------
bool IsPointInFrontOfPlane3D( Vec3 const& point, Plane3 const& plane )
{
	float pointAltitude = DotProduct3D( point, plane.m_normal );
	if ( pointAltitude > plane.m_distanceFromOrigin )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------
int GetRaycastResultFurthestFromStart3D( std::vector<RaycastResult3D> const& raycastList )
{
	int	  resultIndex = -1;
	float maxDistance = 0.f;

	for ( int index = 0; index < raycastList.size(); index++ )
	{
		float impactDistance = raycastList[ index ].m_impactDist;
		if ( impactDistance > maxDistance )
		{
			maxDistance = impactDistance;
			resultIndex = index;
		}
	}

	return resultIndex;
}


//----------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsConvexHull3D( Vec3 const& rayStartPos, Vec3 const& rayFwdNormal, float rayMaxDistance, ConvexHull3 const& convexHull )
{
	RaycastResult3D failResult;
	failResult.m_rayStartPos  = rayStartPos;
	failResult.m_rayFwdNormal = rayFwdNormal;
	failResult.m_rayMaxLength = rayMaxDistance;
	failResult.m_didImpact	  = false;


	// 1. check if entry point is inside the convex plane
	if ( IsPointInsideConvexHull3D( rayStartPos, convexHull ) )
	{
		RaycastResult3D hitResult;
		hitResult.m_rayStartPos	 = rayStartPos;
		hitResult.m_rayFwdNormal = rayFwdNormal;
		hitResult.m_rayMaxLength = rayMaxDistance;
		hitResult.m_didImpact	 = true;
		hitResult.m_impactPos	 = rayStartPos;
		hitResult.m_impactDist	 = 0.f;
		hitResult.m_impactNormal = -1.f * rayFwdNormal;

		// hit
		return hitResult;
	}


	// 2. check raycast with all planes
	std::vector<RaycastResult3D> entryRaycastResults;
	std::vector<RaycastResult3D> exitRaycastResults;
	for ( int index = 0; index < convexHull.m_boundingPlanes.size(); index++ )
	{
		Plane3 const&	plane  = convexHull.m_boundingPlanes[ index ];
		RaycastResult3D result = RaycastVsPlane3D( rayStartPos, rayFwdNormal, rayMaxDistance, plane.m_normal, plane.m_distanceFromOrigin );
		if ( result.m_didImpact )
		{
			// is entry or exit point
			bool isStartPointInFrontOfPlane = IsPointInFrontOfPlane3D( rayStartPos, plane );
			if ( isStartPointInFrontOfPlane ) // in front of plane
			{
				entryRaycastResults.push_back( result );
			}
			else // behind plane
			{
				exitRaycastResults.push_back( result );
			}
		}
	}


	// 3. check if no entry point, start is outside the hull
	if ( entryRaycastResults.empty() )
	{
		// miss
		return failResult;
	}


	// 4. check if last entry point is inside the convex hull
	int				furthestIndex		   = GetRaycastResultFurthestFromStart3D( entryRaycastResults );
	RaycastResult3D lastEntryRaycastResult = entryRaycastResults[ furthestIndex ];
	Vec3			lastEntryImpactPos	   = lastEntryRaycastResult.m_impactPos;
	if ( IsPointInsideConvexHull3D( lastEntryImpactPos, convexHull ) )
	{
		// hit
		RaycastResult3D hitResult = lastEntryRaycastResult;
		return hitResult;
	}


	return failResult;
}
