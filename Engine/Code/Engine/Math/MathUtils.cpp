#include "Engine/Math/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <math.h>
#include <cassert>


float ConvertDegreesToRadians( float degrees )
{
	return degrees * ( M_PI / 180.f );
}

float ConvertRadiansToDegrees( float radians )
{
	return radians * ( 180.f / M_PI );
}

float CosDegrees( float degrees )
{
	return cosf( ConvertDegreesToRadians( degrees ) );
}

float SinDegrees( float degrees )
{
	return sinf( ConvertDegreesToRadians( degrees ) );
}

float TanDegrees( float degrees )
{
	return tanf( ConvertDegreesToRadians( degrees ) );
}

float Atan2Degrees( float y, float x )
{
	return ConvertRadiansToDegrees( atan2f( y, x ) );
}

float AsinDegrees( float x )
{
	return ConvertRadiansToDegrees( asinf( x ) );
}

float AcosDegrees( float x )
{
	float inversCosineRadians  = acosf( x );
	float inverseCosineDegrees = ConvertRadiansToDegrees( inversCosineRadians );
	return inverseCosineDegrees;
	// return ConvertRadiansToDegrees( acosf( x ) );
}


float UnwindAngleDownDegrees( float angle )
{
	while ( angle > 360.f )
	{
		angle -= 360.f;
	}

	return angle;
}

float UnwindAngleUpDegrees( float angle )
{
	while ( angle < -360.f )
	{
		angle += 360.f;
	}

	return angle;
}

float UnwindAngleDegrees( float angle )
{
	float unwindedAngle = angle;

	if ( angle < -360.f )
	{
		unwindedAngle = UnwindAngleUpDegrees( angle );
	}
	else if ( angle > 360.f )
	{
		unwindedAngle = UnwindAngleDownDegrees( angle );
	}

	return unwindedAngle;
}


float GetShortestAngularDispDegrees( float startDegrees, float endDegrees )
{
	startDegrees = UnwindAngleDegrees( startDegrees );
	endDegrees	 = UnwindAngleDegrees( endDegrees );

	float delta = endDegrees - startDegrees;

	if ( delta < -180.f )
	{
		delta = 360.f + delta;
	}

	if ( delta > 180.f )
	{
		delta = delta - 360.f;
	}

	return delta;
}

float GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees )
{
	currentDegrees = UnwindAngleDegrees( currentDegrees );
	goalDegrees	   = UnwindAngleDegrees( goalDegrees );

	float delta = GetShortestAngularDispDegrees( currentDegrees, goalDegrees );

	float absoluteDelta = fabsf( delta );
	if ( maxDeltaDegrees > absoluteDelta )
		maxDeltaDegrees = absoluteDelta;

	float turn = currentDegrees;
	if ( delta > 0.f )
	{
		turn += maxDeltaDegrees;
	}
	if ( delta < 0.f )
	{
		turn -= maxDeltaDegrees;
	}

	return turn;
}

// exploits the two formulas of dot product
// A . B = (Ax * Bx) + (Ay * By)
// A . B = |A| * |B| * cos(thetaAB)
float GetAngleDegreesBetweenVectors2D( Vec2 const& vectorA, Vec2 const& vectorB )
{
	float dotProduct	   = DotProduct2D( vectorA, vectorB );
	float vectorAMagnitude = vectorA.GetLength();
	float vectorBMagnitude = vectorB.GetLength();

	float angleRadians = acosf( dotProduct / ( vectorAMagnitude * vectorBMagnitude ) );
	float angleDegrees = ConvertRadiansToDegrees( angleRadians );

	return angleDegrees;
}


//----------------------------------------------------------------------------------------------------------
float GetAngleDegreesBetweenVectors3D( Vec3 const& vectorA, Vec3 const& vectorB )
{
	float dotProduct = DotProduct3D( vectorA, vectorB );

	float lengthA = vectorA.GetLength();
	float lengthB = vectorB.GetLength();

	float cosTheta	   = ( dotProduct / lengthA * lengthB );
	float thetaDegrees = AcosDegrees( cosTheta );

	return thetaDegrees;
}


// Dot and Cross

float DotProduct2D( const Vec2& a, const Vec2& b )
{
	return ( a.x * b.x + a.y * b.y );
}

float DotProduct3D( Vec3 const& a, Vec3 const& b )
{
	return ( a.x * b.x + a.y * b.y + a.z * b.z );
}

float CrossProduct2D( Vec2 const& a, Vec2 const& b )
{
	float cross2D = ( a.x * b.y ) - ( a.y * b.x );
	return cross2D;
}

Vec3 CrossProduct3D( Vec3 const& a, Vec3 const& b )
{
	Vec3 crossProduct;
	crossProduct.x = ( a.y * b.z ) - ( a.z * b.y );
	crossProduct.y = ( a.z * b.x ) - ( a.x * b.z );
	crossProduct.z = ( a.x * b.y ) - ( a.y * b.x );

	return crossProduct;
}

float DotProduct4D( Vec4 const& a, Vec4 const& b )
{
	float x			 = a.x * b.x;
	float y			 = a.y * b.y;
	float z			 = a.z * b.z;
	float w			 = a.w * b.w;
	float dotProduct = x + y + z + w;

	return dotProduct;
}



Vec2 GetNearestPointOnDisc2D( Vec2 const& referencePoint, Vec2 const& center, float radius )
{
	Vec2 displacement = referencePoint - center;

	if ( displacement.GetLength() <= radius )
		return referencePoint;

	displacement.ClampLength( radius );
	displacement += center;

	return displacement;
}

bool PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float discradius, Vec2 const& fixedPoint )
{
	Vec2  displacement = mobileDiscCenter - fixedPoint;
	float distance	   = displacement.GetLength(); // distance between disc center and fixed point

	if ( distance > discradius )
	{
		return false;
	}
	else
	{
		float pushMagnitude = discradius - distance;
		Vec2  pushVector	= displacement;
		if ( pushVector == Vec2::ZERO ) // if disc is on top of fixed point
		{
			pushVector = Vec2::EAST;
			pushVector.SetLength( discradius );
		}
		else
		{
			pushVector.SetLength( pushMagnitude );
		}
		mobileDiscCenter += pushVector;

		return true;
	}
}

bool PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius )
{
	float distanceBetweenCenters = GetDistance2D( mobileDiscCenter, fixedDiscCenter );
	float radiiSum				 = mobileDiscRadius + fixedDiscRadius;
	float overlapDistance		 = radiiSum - distanceBetweenCenters;

	// not overlapping
	if ( overlapDistance <= 0.f )
		return false;

	Vec2 displacementBetweenCenter = mobileDiscCenter - fixedDiscCenter;
	displacementBetweenCenter.SetLength( overlapDistance );

	Vec2 pushVector = displacementBetweenCenter;
	if ( pushVector == Vec2::ZERO )
	{
		pushVector = Vec2::EAST;
		pushVector.SetLength( radiiSum );
	}
	mobileDiscCenter += pushVector;

	return true;
}

bool PushDiscsOutOfEachOther2D( Vec2& discCenterA, float discRadiusA, Vec2& discCenterB, float discRadiusB )
{
	float distanceBetweenCenters = GetDistance2D( discCenterA, discCenterB );
	float radiiSum				 = discRadiusA + discRadiusB;
	float overlapDistance		 = radiiSum - distanceBetweenCenters;

	if ( overlapDistance <= 0.f )
		return false;

	float pushDistance = overlapDistance / 2.f;

	// push direction for discA
	Vec2 pushVectorForDiscA = discCenterA - discCenterB;
	if ( pushVectorForDiscA == Vec2::ZERO )
	{
		pushVectorForDiscA = Vec2( 1.f, 0.f ); // push east
	}
	pushVectorForDiscA.SetLength( pushDistance );
	discCenterA += pushVectorForDiscA;

	// push direction for discB
	Vec2 pushVectorForDiscB = discCenterB - discCenterA;
	if ( pushVectorForDiscB == Vec2::ZERO )
	{
		pushVectorForDiscB = Vec2( -1.f, 0.f ); // push west
	}
	pushVectorForDiscB.SetLength( pushDistance );
	discCenterB += pushVectorForDiscB;

	return true;
}

bool PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float mobileDiscRadius, AABB2 box )
{
	// 1. get nearest point on AABB2D from disc center
	Vec2 nearestPointOnBox = box.GetNearestPoint( mobileDiscCenter );

	// 2. push disc out of that fixed point
	if ( nearestPointOnBox != mobileDiscCenter )
	{
		return PushDiscOutOfFixedPoint2D( mobileDiscCenter, mobileDiscRadius, nearestPointOnBox );
	}
	else
	{
		// find the nearest point on the edge of the box to C
		float distanceToEast  = fabsf( box.m_maxs.x - mobileDiscCenter.x );
		float distanceToWest  = fabsf( box.m_mins.x - mobileDiscCenter.x );
		float distanceToNorth = fabsf( box.m_maxs.y - mobileDiscCenter.y );
		float distanceToSouth = fabsf( box.m_mins.y - mobileDiscCenter.y );

		Vec2  pushDirection;
		float pushMagnitude = 0.f;
		if ( distanceToSouth <= distanceToWest &&
			 distanceToSouth <= distanceToNorth &&
			 distanceToSouth <= distanceToEast )
		{
			pushDirection = Vec2::SOUTH;
			pushMagnitude = distanceToSouth + mobileDiscRadius;
		}
		else if ( distanceToWest <= distanceToEast &&
				  distanceToWest <= distanceToNorth &&
				  distanceToWest <= distanceToSouth )
		{
			pushDirection = Vec2::WEST;
			pushMagnitude = distanceToWest + mobileDiscRadius;
		}
		else if ( distanceToNorth <= distanceToEast &&
				  distanceToNorth <= distanceToWest &&
				  distanceToNorth <= distanceToSouth )
		{
			pushDirection = Vec2::NORTH;
			pushMagnitude = distanceToNorth + mobileDiscRadius;
		}
		else
		{
			pushDirection = Vec2::EAST;
			pushMagnitude = distanceToEast + mobileDiscRadius;
		}

		Vec2 pushVector = pushDirection;
		pushVector.SetLength( pushMagnitude );
		mobileDiscCenter += pushVector;

		return true;
	}
}

bool IsPointInsideOrientedSector2D(
	Vec2 const& point,				   // Point to be checked for inclusion within the sector
	Vec2 const& sectorTip,			   // Center of the circle that the sector is a part of
	float		sectorForwardDegrees,  // Starting angle of the sector (in absolute degrees)
	float		sectorApertureDegrees, // Width of the sector (in relative degrees)
	float		sectorRadius		   // Radius of the circle that the sector is a part of
)
{
	Vec2 sectorForwardNormal = Vec2::MakeFromPolarDegrees( sectorForwardDegrees );
	return IsPointInsideDirectedSector2D( point, sectorTip, sectorForwardNormal, sectorApertureDegrees, sectorRadius );
}

bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius )
{
	// check if point inside circle
	Vec2  displacement = point - sectorTip;
	float distance	   = displacement.GetLength();
	if ( distance > sectorRadius )
		return false;

	// check if point inside sector
	float deltaDegrees = GetAngleDegreesBetweenVectors2D( sectorForwardNormal, displacement );
	if ( deltaDegrees > sectorApertureDegrees * 0.5f )
		return false;

	return true;
}

float GetDistance2D( const Vec2& positionA, const Vec2& positionB )
{
	return sqrtf( ( ( positionA.x - positionB.x ) * ( positionA.x - positionB.x ) ) +
				  ( ( positionA.y - positionB.y ) * ( positionA.y - positionB.y ) ) );
}

float GetDistanceSquared2D( const Vec2& positionA, const Vec2& positionB )
{
	return ( ( positionA.x - positionB.x ) * ( positionA.x - positionB.x ) ) +
		   ( ( positionA.y - positionB.y ) * ( positionA.y - positionB.y ) );
}

float GetDistance3D( const Vec3& positionA, const Vec3& positionB )
{
	return sqrtf( ( ( positionA.x - positionB.x ) * ( positionA.x - positionB.x ) ) +
				  ( ( positionA.y - positionB.y ) * ( positionA.y - positionB.y ) ) +
				  ( ( positionA.z - positionB.z ) * ( positionA.z - positionB.z ) ) );
}

float GetDistanceSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	return ( ( positionA.x - positionB.x ) * ( positionA.x - positionB.x ) ) +
		   ( ( positionA.y - positionB.y ) * ( positionA.y - positionB.y ) ) +
		   ( ( positionA.z - positionB.z ) * ( positionA.z - positionB.z ) );
}

float GetDistanceXY3D( const Vec3& positionA, const Vec3& positionB )
{
	return sqrtf( ( ( positionA.x - positionB.x ) * ( positionA.x - positionB.x ) ) +
				  ( ( positionA.y - positionB.y ) * ( positionA.y - positionB.y ) ) );
}

float GetDistanceXYSquared3D( const Vec3& positionA, const Vec3& positionB )
{
	return ( ( positionA.x - positionB.x ) * ( positionA.x - positionB.x ) ) +
		   ( ( positionA.y - positionB.y ) * ( positionA.y - positionB.y ) );
}

float GetProjectedLength2D( const Vec2& vectorToProject, const Vec2& vectorToProjectOnto )
{
	Vec2  normalizedVectorToProjectOnto = vectorToProjectOnto.GetNormalized();
	float projectedLength				= DotProduct2D( vectorToProject, normalizedVectorToProjectOnto );

	return projectedLength;
}

float GetProjectedLength3D( const Vec3& vectorToProject, const Vec3& vectorToProjectOnto )
{
	Vec3  normalizedVectorToProjectOnto = vectorToProjectOnto.GetNormalized();
	float projectedLength				= DotProduct3D( vectorToProject, normalizedVectorToProjectOnto );

	return projectedLength;
}

Vec2 GetProjectedOnto2D( const Vec2& vectorToProject, const Vec2& vectorToProjectOnto )
{
	float projectedlenght				= GetProjectedLength2D( vectorToProject, vectorToProjectOnto );
	Vec2  normalizedVectorToProjectOnto = vectorToProjectOnto.GetNormalized();
	Vec2  projectedVector				= projectedlenght * normalizedVectorToProjectOnto;

	return projectedVector;
}

Vec3 GetProjectedOnto3D( const Vec3& vectorToProject, const Vec3& vectorToProjectOnto )
{
	float projectedlenght				= GetProjectedLength3D( vectorToProject, vectorToProjectOnto );
	Vec3  normalizedVectorToProjectOnto = vectorToProjectOnto.GetNormalized();
	Vec3  projectedVector				= projectedlenght * normalizedVectorToProjectOnto;

	return projectedVector;
}

int GetTaxicabDistance2D( IntVec2 const& positionA, IntVec2 const& positionB )
{
	int xDistance = positionB.x - positionA.x;
	int yDistance = positionB.y - positionA.y;

	return abs( yDistance ) + abs( xDistance );
}

bool DoDiscsOverlap( const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB )
{
	float distanceBetweenCenterSquared = GetDistanceSquared2D( centerA, centerB );
	float squareOfSumOfRadii		   = ( radiusA + radiusB ) * ( radiusA + radiusB );

	if ( distanceBetweenCenterSquared < squareOfSumOfRadii )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DoSpheresOverlap( const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB )
{
	return GetDistance3D( centerA, centerB ) < ( radiusA + radiusB );
}

void TransformPosition2D( Vec2& positionToTranform, float uniformScale, float rotationDegrees, const Vec2& translation )
{
	positionToTranform *= uniformScale;

	positionToTranform.RotateDegrees( rotationDegrees );

	positionToTranform += translation;
}

void TransformPosition2D( Vec2& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
	positionToTransform = ( positionToTransform.x * iBasis ) + ( positionToTransform.y * jBasis );

	positionToTransform += translation;
}

void TransformPositionXY3D( Vec3& positionToTranform, float scaleXY, float zRotationDegrees, const Vec2& translationXY )
{
	positionToTranform.SetScaleXY( scaleXY );

	positionToTranform.RotateAboutZRadians( ConvertDegreesToRadians( zRotationDegrees ) );

	positionToTranform.x += translationXY.x;
	positionToTranform.y += translationXY.y;
}

void TransformPositionXY3D( Vec3& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
	Vec2 positionToTransform2D( positionToTransform.x, positionToTransform.y );
	TransformPosition2D( positionToTransform2D, iBasis, jBasis, translation );

	positionToTransform.x = positionToTransform2D.x;
	positionToTransform.y = positionToTransform2D.y;
}

float GetClamped( float value, float minValue, float maxValue )
{
	if ( value > maxValue )
		return maxValue;
	if ( value < minValue )
		return minValue;

	return value;
}

int GetClampedInt( int value, int minValue, int maxValue )
{
	if ( value > maxValue )
		return maxValue;
	if ( value < minValue )
		return minValue;

	return value;
}

float GetClampedZeroToOne( float value )
{
	if ( value > 1.f )
		return 1.f;
	if ( value < 0.f )
		return 0.f;

	return value;
}

float Interpolate( float start, float end, float fractionTowardsEnd )
{
	float delta = end - start;
	float value = start + ( delta * fractionTowardsEnd );

	return value;
}

// TODO: add check for devide by zero
float GetFractionWithinRange( float value, float rangeStart, float rangeEnd )
{
	float delta	   = rangeEnd - rangeStart;
	float fraction = ( value - rangeStart ) / delta;

	return fraction;
}

float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	float inFraction = GetFractionWithinRange( inValue, inStart, inEnd );
	float outValue	 = Interpolate( outStart, outEnd, inFraction );

	return outValue;
}


//----------------------------------------------------------------------------------------------------------
float RangeMap(float inValue, FloatRange inRange, FloatRange outRange )
{
	return RangeMap( inValue, inRange.GetMin(), inRange.GetMax(), outRange.GetMin(), outRange.GetMax() );
}


//----------------------------------------------------------------------------------------------------------
Vec2 RangeMap( Vec2 inValue, AABB2 inBounds, AABB2 outBounds )
{
	Vec2 outValue;
	outValue.x = RangeMap( inValue.x, inBounds.GetXRange(), outBounds.GetXRange() );
	outValue.y = RangeMap( inValue.y, inBounds.GetYRange(), outBounds.GetYRange() );

	return outValue;
}


//----------------------------------------------------------------------------------------------------------
float RangeMapClamped( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	float rangeMappedValue = RangeMap( inValue, inStart, inEnd, outStart, outEnd );

	float minValue = outStart;
	float maxValue = outEnd;
	if ( outStart > outEnd )
	{
		minValue = outEnd;
		maxValue = outStart;
	}
	float clamedValue = GetClamped( rangeMappedValue, minValue, maxValue );

	return clamedValue;
}


//----------------------------------------------------------------------------------------------------------
Quaternion NormalizedLerp( float inValue, float inStart, float inEnd, Quaternion const& outStart, Quaternion const& outEnd )
{
	// 1. make sure the quaternions are on the same hemisphere
	Quaternion A = outStart;
	Quaternion B = outEnd;

	Vec4  outStartAsVec4 = A.GetAsVec4();
	Vec4  outEndAsVec4	 = B.GetAsVec4();
	float dotProduct	 = DotProduct4D( outStartAsVec4, outEndAsVec4 );
	if ( dotProduct < 0.f )
	{
		B = B * -1.f;
	}

	// 2. lerp between quaternion A and B
	float	   inFraction = GetFractionWithinRange( inValue, inStart, inEnd );
	Quaternion lerpResult = A + ( ( B - A ) * inFraction );

	// 3. normalize the result
	lerpResult.Normalize();

	return lerpResult;
}



int RoundDownToInt( float value )
{
	return static_cast<int>( floorf( value ) );
}


// new methods for Math Visual Tests
// TODO: organize methods according to functionality later

// is point inside XXX -------------------------------------------------------

bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius )
{
	Vec2  displacement		= point - discCenter;
	float discRadiusSquared = discRadius * discRadius;

	if ( displacement.GetLengthSquared() <= discRadiusSquared )
		return true;

	return false;
}


// bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box)
//{
//	UNUSED(point);
//	UNUSED(box);
//
//	return false;
// }

bool IsPointInsideCapsule2D( Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	Vec2 nearestPointOnBone = GetNearestPointOnLineSegment2D( point, boneStart, boneEnd );
	Vec2 NP					= point - nearestPointOnBone;

	if ( NP.GetLength() > radius )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool IsPointInsideCapsule3D( Vec3 const& point, Vec3 const& boneStart, Vec3 const& boneEnd, float radius )
{
	Vec3 nearestPointOnBone = GetNearestPointOnLineSegment3D( point, boneStart, boneEnd );
	Vec3 NP					= point - nearestPointOnBone;

	if ( NP.GetLength() > radius )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool IsPointInsideOBB2D( Vec2 const& point, OBB2 const& orientedBox )
{
	// oriented box data
	Vec2 const& center	   = orientedBox.m_center;
	Vec2 const& i		   = orientedBox.m_iBasisNormal;
	Vec2		j		   = i.GetRotated90Degrees();
	float		halfWidth  = orientedBox.m_halfDimensions.x;
	float		halfHeight = orientedBox.m_halfDimensions.y;

	// calculate projections on i & j normals
	Vec2  vecCenterToPoint = point - center;
	float projectedOntoI   = GetProjectedLength2D( vecCenterToPoint, i );
	float projectedOntoJ   = GetProjectedLength2D( vecCenterToPoint, j );

	// width check
	if ( fabsf( projectedOntoI ) > halfWidth )
	{
		return false;
	}

	// height check
	if ( fabsf( projectedOntoJ ) > halfHeight )
	{
		return false;
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------
bool IsPointInsideHexagon2D( Vec2 const& point, Vec2 const& hexCenter, float hexInRadius, float hexCircumRadius )
{
	UNUSED( hexInRadius );

	// calculate all hexagon points
	float const STARTING_ANGLE			 = 0.f;
	float const HEX_TRIANGLE_ARC_DEGREES = 60.f;

	Vec2 hexCorner0 = Vec2::MakeFromPolarDegrees( 0.f, hexCircumRadius );
	Vec2 hexCorner1 = hexCorner0.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner2 = hexCorner1.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner3 = hexCorner2.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner4 = hexCorner3.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner5 = hexCorner4.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );

	hexCorner0 += hexCenter;
	hexCorner1 += hexCenter;
	hexCorner2 += hexCenter;
	hexCorner3 += hexCenter;
	hexCorner4 += hexCenter;
	hexCorner5 += hexCenter;

	// walk around the hexagon and check if point on the same side
	// hexcorner0
	Vec2  edgeVectorVertex0 = hexCorner1 - hexCorner0;
	Vec2  vertex0ToPoint	= point - hexCorner0;
	float crossProduct0		= CrossProduct2D( edgeVectorVertex0, vertex0ToPoint );
	if ( crossProduct0 < 0.f )
	{
		return false;
	}

	// hexcorner1
	Vec2  edgeVectorVertex1 = hexCorner2 - hexCorner1;
	Vec2  vertex1ToPoint	= point - hexCorner1;
	float crossProduct1		= CrossProduct2D( edgeVectorVertex1, vertex1ToPoint );
	if ( crossProduct1 < 0.f )
	{
		return false;
	}

	// hexcorner2
	Vec2  edgeVectorVertex2 = hexCorner3 - hexCorner2;
	Vec2  vertex2ToPoint	= point - hexCorner2;
	float crossProduct2		= CrossProduct2D( edgeVectorVertex2, vertex2ToPoint );
	if ( crossProduct2 < 0.f )
	{
		return false;
	}

	// hexcorner3
	Vec2  edgeVectorVertex3 = hexCorner4 - hexCorner3;
	Vec2  vertex3ToPoint	= point - hexCorner3;
	float crossProduct3		= CrossProduct2D( edgeVectorVertex3, vertex3ToPoint );
	if ( crossProduct3 < 0.f )
	{
		return false;
	}

	// hexcorner4
	Vec2  edgeVectorVertex4 = hexCorner5 - hexCorner4;
	Vec2  vertex4ToPoint	= point - hexCorner4;
	float crossProduct4		= CrossProduct2D( edgeVectorVertex4, vertex4ToPoint );
	if ( crossProduct4 < 0.f )
	{
		return false;
	}

	// hexcorner5
	Vec2  edgeVectorVertex5 = hexCorner0 - hexCorner5;
	Vec2  vertex5ToPoint	= point - hexCorner5;
	float crossProduct5		= CrossProduct2D( edgeVectorVertex5, vertex5ToPoint );
	if ( crossProduct5 < 0.f )
	{
		return false;
	}

	return true;
}


// get nearest point on XXX ----------------------------------------------------------------
// Vec2 const GetNearestPointOnAABB2D(Vec2 const& referencePos, AABB2 const& box)
//{
//	UNUSED(referencePos);
//	UNUSED(box);
//
//
//	return Vec2();
//}


Vec2 const GetNearestPointOnInfiniteLine2D( Vec2 const& referncePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine )
{
	Vec2 lineSegment		= anotherPointOnLine - pointOnLine;
	Vec2 vecStartToRefPoint = referncePos - pointOnLine;

	Vec2 projectedVec		= GetProjectedOnto2D( vecStartToRefPoint, lineSegment );
	Vec2 nearestPointOnLine = pointOnLine + projectedVec;
	return nearestPointOnLine;
}

Vec2 const GetNearestPointOnLineSegment2D( Vec2 const& refPoint, Vec2 const& startPoint, Vec2 const& endPoint )
{
	Vec2 lineSegment		= endPoint - startPoint;
	Vec2 vecStartToRefPoint = refPoint - startPoint;
	Vec2 vecEndToRefPoint	= refPoint - endPoint;

	// check if point is on the voronoi left region of line
	float dotProductS = DotProduct2D( lineSegment, vecStartToRefPoint );
	if ( dotProductS < 0.f )
	{
		return startPoint;
	}

	// check if point is on the vornoi right region of line
	float dotProductE = DotProduct2D( lineSegment, vecEndToRefPoint );
	if ( dotProductE > 0.f )
	{
		return endPoint;
	}

	// point in inside vornoi region of line segment
	Vec2 projectedVec = GetProjectedOnto2D( vecStartToRefPoint, lineSegment );
	Vec2 pointOnLine  = startPoint + projectedVec;
	return pointOnLine;
}

Vec3 const GetNearestPointOnLineSegment3D( Vec3 const& referencePos, Vec3 const& lineSegStart, Vec3 const& lineSegEnd )
{
	Vec3 lineSegment		= lineSegEnd - lineSegStart;
	Vec3 vecStartToRefPoint = referencePos - lineSegStart;
	Vec3 vecEndToRefPoint	= referencePos - lineSegEnd;

	// check if point is on the voronoi left region of line
	float dotProductS = DotProduct3D( lineSegment, vecStartToRefPoint );
	if ( dotProductS < 0.f )
	{
		return lineSegStart;
	}

	// check if point is on the voronoi right region of line
	float dotProductE = DotProduct3D( lineSegment, vecEndToRefPoint );
	if ( dotProductE > 0.f )
	{
		return lineSegEnd;
	}

	// point in inside voronoi region of line segment
	Vec3 projectedVec = GetProjectedOnto3D( vecStartToRefPoint, lineSegment );
	Vec3 pointOnLine  = lineSegStart + projectedVec;
	return pointOnLine;
}

Vec2 const GetNearestPointOnCapsule2D( Vec2 const& referncePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	Vec2 nearestPointOnCapsule;
	Vec2 nearestPointOnBone = GetNearestPointOnLineSegment2D( referncePos, boneStart, boneEnd );

	Vec2 NP = referncePos - nearestPointOnBone;
	NP.ClampLength( radius );

	nearestPointOnCapsule = nearestPointOnBone + NP;

	return nearestPointOnCapsule;
}

Vec2 const GetNearestPointOnOBB2D( Vec2 const& referncePoint, OBB2 const& orientedBox )
{
	if ( IsPointInsideOBB2D( referncePoint, orientedBox ) )
		return referncePoint;

	// oriented box data
	Vec2 const& center	   = orientedBox.m_center;
	Vec2 const& i		   = orientedBox.m_iBasisNormal;
	Vec2		j		   = i.GetRotated90Degrees();
	float		halfWidth  = orientedBox.m_halfDimensions.x;
	float		halfHeight = orientedBox.m_halfDimensions.y;

	// calculate projections on i & j normals
	Vec2  vecCenterToPoint = referncePoint - center;
	float projectedOntoI   = GetProjectedLength2D( vecCenterToPoint, i );
	float projectedOntoJ   = GetProjectedLength2D( vecCenterToPoint, j );
	Vec2  refPointIJ( projectedOntoI, projectedOntoJ );

	float nearestpointX = GetClamped( refPointIJ.x, -halfWidth, halfWidth );
	float nearestpointY = GetClamped( refPointIJ.y, -halfHeight, halfHeight );

	Vec2 nearestpointIJ( nearestpointX, nearestpointY );
	TransformPosition2D( nearestpointIJ, i, j, center );

	return nearestpointIJ;
}



// Normalize & De-Normalize Byte

float NormalizeByte( unsigned char byteValue )
{
	float fractionPerByte = 1.f / 255.f;

	float normalizedByte = fractionPerByte * byteValue;
	return normalizedByte;
}

unsigned char DenormalizeByte( float zeroToOne )
{
	float numBytes		 = ( 255.f - 0.f ) + 1.f;
	float bracketPercent = 1.f / numBytes;
	int	  bracket		 = ( int ) ( zeroToOne / bracketPercent );
	if ( bracket > 255 )
		bracket = 255;

	unsigned char denormalizedByte = ( unsigned char ) bracket;
	return denormalizedByte;
}


Mat44 GetBillboardMatrix( BillboardType billboardType, Mat44 const& cameraMatrix, Vec3 const& billboardPosition, Vec2 const& billboardScale )
{
	Mat44 transform;
	if ( billboardType == BillboardType::FULL_CAMERA_OPPOSING )
	{
		Vec3 iBasis = -1.f * cameraMatrix.GetIBasis3D();
		Vec3 jBasis = -1.f * cameraMatrix.GetJBasis3D();
		Vec3 kBasis = cameraMatrix.GetKBasis3D();
		kBasis.Normalize();

		transform.SetIJK3D( iBasis, jBasis, kBasis );
	}
	else if ( billboardType == BillboardType::WORLD_UP_CAMERA_FACING )
	{
		Vec3 kBasis = Vec3( 0.f, 0.f, 1.f );

		Vec3 cameraPosition			= cameraMatrix.GetTranslation3D();
		Vec3 directionTowardsCamera = cameraPosition - billboardPosition;
		directionTowardsCamera.z	= 0.f;
		directionTowardsCamera.Normalize();
		Vec3 iBasis = directionTowardsCamera;

		Vec3 jBasis = CrossProduct3D( kBasis, iBasis );
		jBasis.Normalize();

		transform.SetIJK3D( iBasis, jBasis, kBasis );
	}
	else if ( billboardType == BillboardType::WORLD_DOWN_CAMERA_FACING )
	{
		Vec3 kBasis = Vec3( 0.f, 0.f, -1.f );

		Vec3 cameraPosition			= cameraMatrix.GetTranslation3D();
		Vec3 directionTowardsCamera = cameraPosition - billboardPosition;
		directionTowardsCamera.z	= 0.f;
		directionTowardsCamera.Normalize();
		Vec3 iBasis = directionTowardsCamera;

		Vec3 jBasis = CrossProduct3D( kBasis, iBasis );
		jBasis.Normalize();

		transform.SetIJK3D( iBasis, jBasis, kBasis );
	}
	else if ( billboardType == BillboardType::WORLD_UP_CAMERA_OPPOSING )
	{
		Vec3 kBasis = Vec3( 0.f, 0.f, 1.f );

		Vec3 iBasis = -1.f * cameraMatrix.GetIBasis3D();
		iBasis.z	= 0.f;
		iBasis.Normalize();

		Vec3 jBasis = CrossProduct3D( kBasis, iBasis );
		jBasis.Normalize();

		transform.SetIJK3D( iBasis, jBasis, kBasis );
	}

	Vec3 yzScale = Vec3( 1.f, billboardScale.x, billboardScale.y );
	transform.AppendScaleNonUniform3D( yzScale );

	return transform;
}


void BounceDiscOffEachOther2D( Vec2& positionA, float radiusA, Vec2& velocityA,
	Vec2& positionB, float radiusB, Vec2& velocityB,
	float elasticityA, float elasticityB )
{
	bool doDiscOverlap = DoDiscsOverlap( positionA, radiusA, positionB, radiusB );

	if ( doDiscOverlap )
	{
		PushDiscsOutOfEachOther2D( positionA, radiusA, positionB, radiusB );



		// check is velocity is divergent or convergent
		// Vec2 normalAtoB = positionB - positionA;
		// normalAtoB.Normalize();
		// Vec2 normalizedVelocityA = velocityA.GetNormalized();
		// float dotProductVelocityA = DotProduct2D(normalAtoB, normalizedVelocityA);

		// Vec2 normalBtoA = -1.f * normalAtoB;
		// Vec2 normalVelocityB = velocityB.GetNormalized();
		// float dotProductVelocityB = DotProduct2D(normalBtoA, normalVelocityB);

		// if ( dotProductVelocityA < 0.f && dotProductVelocityB < 0.f )
		//{
		//	return;	// divergent, do not exchange velocity
		// }

		float elasticity = elasticityA * elasticityB;

		Vec2 normal = positionB - positionA;
		normal.Normalize();

		Vec2 velocityANormal  = GetProjectedOnto2D( velocityA, normal );
		Vec2 velocityATangent = velocityA - velocityANormal;

		Vec2 velocityBNormal  = GetProjectedOnto2D( velocityB, normal );
		Vec2 velocityBTangent = velocityB - velocityBNormal;

		Vec2  deltaPosition	   = positionB - positionA;
		Vec2  relativeVelocity = velocityB - velocityA;
		float dotProduct	   = DotProduct2D( deltaPosition, relativeVelocity );

		if ( dotProduct < 0.f )
		{
			Vec2 bounceVelocityA = ( velocityBNormal * elasticity ) + velocityATangent;
			velocityA			 = bounceVelocityA;

			Vec2 bounceVelocityB = ( velocityANormal * elasticity ) + velocityBTangent;
			velocityB			 = bounceVelocityB;
		}
		else
		{
			/*Vec2 bounceVelocityA = ( velocityANormal * elasticity ) + velocityATangent;
			velocityA = bounceVelocityA;

			Vec2 bounceVelocityB = ( velocityBNormal * elasticity ) + velocityBTangent;
			velocityB = bounceVelocityB;*/
		}
	}
}

void BounceDiscOffFixedDisc2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel,
	Vec2 const& fixedDiscCenter, float fixedDiscRadius,
	float mobileDiscElasticity, float fixedDiscElasticity )
{
	bool doDiscOverLap = DoDiscsOverlap( mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius );

	if ( doDiscOverLap )
	{
		// 1. push
		PushDiscOutOfFixedDisc2D( mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius );

		// check is velocity is divergent or convergent
		Vec2 normalMobile = fixedDiscCenter - mobileDiscCenter;
		normalMobile.Normalize();
		Vec2  normalizedVelocity		  = mobileDiscVel.GetNormalized();
		float dotProductVelocityAndNormal = DotProduct2D( normalMobile, normalizedVelocity );

		if ( dotProductVelocityAndNormal < 0.f )
		{
			return; // divergent, do not exchange velocity
		}


		// 2. change velocity
		float elasticity = mobileDiscElasticity * fixedDiscElasticity;

		Vec2 normal = fixedDiscCenter - mobileDiscCenter;
		normal.Normalize();

		// calculation for mobile disc
		Vec2 Vn = GetProjectedOnto2D( mobileDiscVel, normal );
		Vec2 Vt = mobileDiscVel - Vn;

		Vec2 bouncedVel = Vt - ( Vn * elasticity );
		mobileDiscVel	= bouncedVel;
	}
}



// 1D Bezier ----------------------------------------------------------------------------------------

float Lerp( float A, float B, float t )
{
	float s = 1.f - t;
	float P = ( s * A ) + ( t * B );

	return P;
}


//----------------------------------------------------------------------------------------------------------
Vec2 Lerp( Vec2 const& A, Vec2 const& B, float parametricZeroToOne )
{
	float x = Lerp( A.x, B.x, parametricZeroToOne );
	float y = Lerp( A.y, B.y, parametricZeroToOne );

	return Vec2( x, y );
}


//----------------------------------------------------------------------------------------------------------
Vec3 Lerp( Vec3 const& A, Vec3 const& B, float parametricZeroToOne )
{
	float x = Lerp( A.x, B.x, parametricZeroToOne );
	float y = Lerp( A.y, B.y, parametricZeroToOne );
	float z = Lerp( A.z, B.z, parametricZeroToOne );

	return Vec3( x, y, z );
}


//----------------------------------------------------------------------------------------------------------
Quaternion NormalizedLerp( Quaternion const& A, Quaternion const& B, float parametricZeroToOne )
{
	Quaternion LerpResult = ( A + ( ( B - A ) * parametricZeroToOne ) );
	LerpResult.Normalize();

	return LerpResult;
}


//----------------------------------------------------------------------------------------------------------
Quaternion SphericalLerp( Quaternion const& A, Quaternion const& B, float parametricZeroToOne )
{
	Quaternion p = A;
	Quaternion q = B;

	float dotProduct = DotProduct4D( A.GetAsVec4(), B.GetAsVec4() );

	// make sure to take the shortest arc for interpolation
	if ( dotProduct < 0.f )
	{
		q = -q;
		dotProduct = -dotProduct;
	}

	// if very close to each other, use NLERP
	if ( dotProduct > 0.9999f )
	{
		Quaternion nLerpedResut = NormalizedLerp( p, q, parametricZeroToOne );
		return nLerpedResut;
	}

	// SLerp, refer: https://gamemath.com/book/orient.html#quaternion_slerp
	float cosHalfRadians = dotProduct;
	float sinHalfRadians = sqrtf( 1.f - ( cosHalfRadians * cosHalfRadians ) );

	float halfRadiansAngle		  = atan2f( sinHalfRadians, cosHalfRadians );
	float sintTHalfRadins		  = sinf( parametricZeroToOne * halfRadiansAngle );
	float sinOneMinusTHalfRadinas = sinf( ( 1.f - parametricZeroToOne ) * halfRadiansAngle );
	float oneOverSinHalfRadins	  = 1.f / sinHalfRadians;

	// compute interpolation parameters
	float t0 = sinOneMinusTHalfRadinas * oneOverSinHalfRadins;
	float t1 = sintTHalfRadins * oneOverSinHalfRadins;
	
	Quaternion sLerpedResult = (p * t0) + (q * t1);
	return sLerpedResult;
}


//----------------------------------------------------------------------------------------------------------
Transform LerpTransform( Transform const& A, Transform const& B, float parametricZeroToOne )
{
	// make sure that interpolation between two quaternions is the shortest path
	Quaternion bRotattion		  = B.m_rotation;
	Vec4	   bRotationVec4	  = bRotattion.GetAsVec4();
	Quaternion aRotattion		  = A.m_rotation;
	Vec4	   aRotationVec4	  = aRotattion.GetAsVec4();
	float	   rotationDotProduct = DotProduct4D( aRotationVec4, bRotationVec4 );
	if ( rotationDotProduct < 0.f )
	{
		bRotattion = -bRotattion;
	}

	Vec3	   positionLerp = Lerp( A.m_position, B.m_position, parametricZeroToOne );
	Quaternion rotationLerp = NormalizedLerp( A.m_rotation, bRotattion, parametricZeroToOne );
	Vec3	   scaleLerp	= Lerp( A.m_scale, B.m_scale, parametricZeroToOne );

	return Transform( positionLerp, rotationLerp, scaleLerp );
}



float ComputeQuadraticBezier1D( float A, float B, float C, float t )
{
	float D = Lerp( A, B, t );
	float E = Lerp( B, C, t );
	float P = Lerp( D, E, t );

	return P;
}

float ComputeCubicBezier1D( float A, float B, float C, float D, float t )
{

	float s = 1.f - t;
	float P = ( s * s * s * A ) + ( 3.f * s * s * t * B ) + ( 3.f * s * t * t * C ) + ( t * t * t * D );

	return P;
}

float ComputeQuinticBezier1D( float A, float B, float C, float D, float E, float F, float t )
{
	// Lerp
	float G = Lerp( A, B, t );
	float H = Lerp( B, C, t );
	float I = Lerp( C, D, t );
	float J = Lerp( D, E, t );
	float K = Lerp( E, F, t );

	// lerp of lerps
	float L = Lerp( G, H, t );
	float M = Lerp( H, I, t );
	float N = Lerp( I, J, t );
	float O = Lerp( J, K, t );

	// lerp of lerp of lerps
	float Q = Lerp( L, M, t );
	float R = Lerp( M, N, t );
	float S = Lerp( N, O, t );

	// lerp of lerp of lerps
	float T = Lerp( Q, R, t );
	float U = Lerp( R, S, t );

	float P = Lerp( T, U, t );

	return P;
}


float AngleBetweenQuaternions( Quaternion const& q1, Quaternion const& q2 )
{
	float dotProduct = DotProduct4D( q1.GetAsVec4(), q2.GetAsVec4() );



	float q1Length = q1.GetLength();
	float q2Length = q2.GetLength();



	float cosTheta = ( dotProduct / q1Length * q2Length );

	// float inverseCosine = acosf(-1.2f);

	float thetaDegrees = AcosDegrees( cosTheta );

	// if ( isnan( thetaDegrees ) )
	//{
	//	DebuggerPrintf( "thetaDegrees Is NAN" );
	//	//thetaDegrees = AcosDegrees( cosTheta );
	// }

	return thetaDegrees;
}


//----------------------------------------------------------------------------------------------------------
Quaternion RotateTowards( Quaternion const& startOrientation, Quaternion endOrientation, float maxAngleDegrees )
{
	if ( maxAngleDegrees < 0.001f )
	{
		return startOrientation;
	}

	if ( startOrientation == endOrientation )
	{
		return startOrientation;
	}

	float dotProduct = DotProduct4D( startOrientation.GetAsVec4(), endOrientation.GetAsVec4() );
	if ( dotProduct > 0.9999f )
	{
		return endOrientation;
	}

	if ( dotProduct < 0.f )
	{
		endOrientation = endOrientation * -1.f;
	}
	dotProduct = DotProduct4D( startOrientation.GetAsVec4(), endOrientation.GetAsVec4() );
	if ( dotProduct > 0.9999f )
	{
		return endOrientation;
	}


	// get angle between quaternions
	float thetaDegrees = AngleBetweenQuaternions( startOrientation, endOrientation );



	if ( thetaDegrees <= 0.001f )
	{
		return startOrientation;
	}

	if ( thetaDegrees < maxAngleDegrees )
	{
		return endOrientation;
	}

	// DebuggerPrintf( "theta Degrees: %.2f  ", thetaDegrees );

	float t = maxAngleDegrees / thetaDegrees;

	// DebuggerPrintf( "t: %.2f  \n", t );


	float maxAngleRadians = ConvertDegreesToRadians( maxAngleDegrees );

	// SLerp
	Quaternion sLerpResult = startOrientation * ( sinf( ( 1.f - t ) * maxAngleRadians ) / sinf( maxAngleRadians ) ) +
							 endOrientation * ( sinf( t * maxAngleRadians ) / sinf( maxAngleRadians ) );
	sLerpResult.Normalize();
	return sLerpResult;
}



//----------------------------------------------------------------------------------------------------------
float EPSILON = 0.00001f;
bool  IsPointInsideConvexHull2D( Vec2 const& point, ConvexHull2 const& convexHull2, Vec2 const& boundingDiscCenter, float boundingDiscRadius )
{
	// bounding disc check
	bool isPointInsideBoundingDisc = IsPointInsideDisc2D( point, boundingDiscCenter, boundingDiscRadius );
	if ( !isPointInsideBoundingDisc )
		return false;

	// plane check
	for ( int index = 0; index < convexHull2.m_boundingPlanes.size(); index++ )
	{
		Plane2 const& plane = convexHull2.m_boundingPlanes[ index ];

		float pointAltitude = DotProduct2D( point, plane.m_normal );
		if ( pointAltitude > plane.m_distanceFromOrigin + EPSILON )
		{
			return false;
		}
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------
bool IsPointInsideConvexHull3D( Vec3 const& point, ConvexHull3 const& convexHull3 )
{
	for ( int index = 0; index < convexHull3.m_boundingPlanes.size(); index++ )
	{
		Plane3 const& plane = convexHull3.m_boundingPlanes[ index ];

		float pointAltitude = DotProduct3D( point, plane.m_normal );
		if ( pointAltitude > plane.m_distanceFromOrigin + EPSILON )
		{
			return false;
		}
	}

	return true;
}
