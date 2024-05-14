#pragma once


#include "Engine/Math/ConvexHull3.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Vec4.hpp"

struct Vec2;
struct Vec3;
struct Mat44;


// distance & position
float GetDistance2D(const Vec2& positionA, const Vec2& positionB);
float GetDistanceSquared2D(const Vec2& positionA, const Vec2& positionB);
float GetDistance3D(const Vec3& positionA, const Vec3& positionB);
float GetDistanceSquared3D(const Vec3& positionA, const Vec3& positionB);
float GetDistanceXY3D(const Vec3& positionA, const Vec3& positionB);
float GetDistanceXYSquared3D(const Vec3& positionA, const Vec3& positionB);
float GetProjectedLength2D(const Vec2& vectorToProject, const Vec2& vectorToProjectOnto);
float GetProjectedLength3D(const Vec3& vectorToProject, const Vec3& vectorToProjectOnto);
Vec2 GetProjectedOnto2D(const Vec2& vectorToProject, const Vec2& vectorToProjectOnto);
Vec3 GetProjectedOnto3D(const Vec3& vectorToProject, const Vec3& vectorToProjectOnto);
int GetTaxicabDistance2D(IntVec2 const& positionA, IntVec2 const& positionB);

// collision
bool DoDiscsOverlap(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB);
bool DoSpheresOverlap(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB);

// translate
void TransformPosition2D(Vec2& positionToTranform, float uniformScale, float rotationDegrees, const Vec2& translation);
void TransformPositionXY3D(Vec3& positionToTranform, float scaleXY, float zRotationDegrees, const Vec2& translationXY);
void TransformPosition2D(Vec2& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void TransformPositionXY3D(Vec3& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);

// Clamp and Lerp
float GetClamped(float value, float minValue, float maxValue);
int GetClampedInt(int value, int minValue, int maxValue);
float GetClampedZeroToOne(float value);
float Interpolate(float start, float end, float fractionTowardsEnd);
float GetFractionWithinRange(float value, float rangeStart, float rangeEnd);
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float RangeMap(float inValue, FloatRange inRange, FloatRange outRange );
Vec2 RangeMap( Vec2 inValue, AABB2 inBounds, AABB2 outBounds );
float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int RoundDownToInt(float value);
Quaternion NormalizedLerp(float inValue, float inStart, float inEnd, Quaternion const& outStart, Quaternion const& outEnd);


// Angle Utilities
float ConvertDegreesToRadians(float degrees);
float ConvertRadiansToDegrees(float radians);
float CosDegrees(float degrees);
float SinDegrees(float degrees);
float TanDegrees( float degrees );
float Atan2Degrees(float y, float x);
float AsinDegrees(float x);
float AcosDegrees( float x );
float GetShortestAngularDispDegrees( float startDegrees, float endDegrees );
float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);
float GetAngleDegreesBetweenVectors2D(Vec2 const& vectorA, Vec2 const& vectorB);
float GetAngleDegreesBetweenVectors3D( Vec3 const& vectorA, Vec3 const& vectorB );

// Dot and Cross
float DotProduct2D(const Vec2& a, const Vec2& b);
float DotProduct3D( Vec3 const& a, Vec3 const& b);
float CrossProduct2D( Vec2 const& a, Vec2 const& b );
Vec3 CrossProduct3D(Vec3 const& a, Vec3 const& b);
float DotProduct4D(Vec4 const& a, Vec4 const& b);

// Geometric queries
Vec2 GetNearestPointOnDisc2D(Vec2 const& referencePoint, Vec2 const& center, float radius);
bool PushDiscOutOfFixedPoint2D(Vec2 & mobileDiscCenter, float discradius, Vec2 const& fixedPoint);
bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius);
bool PushDiscsOutOfEachOther2D(Vec2& discCenterA, float discRadiusA, Vec2& discCenterB, float discRadiusB);
bool PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float mobileDiscRadius, AABB2 box);
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius);
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius);

// new methods for Math Visual Tests
// TODO: organize methods according to functionality later

// is point inside XXX
bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius);
//bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box);
bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
bool IsPointInsideCapsule3D(Vec3 const& point, Vec3 const& boneStart, Vec3 const& boneEnd, float radius);
bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& orientedBox);
bool IsPointInsideHexagon2D( Vec2 const& point, Vec2 const& hexCenter, float hexInRadius, float hexCircuRadius );
bool IsPointInsideConvexHull2D(Vec2 const& point, ConvexHull2 const& convexHull2, Vec2 const& boundingDiscCenter, float boundingDiscRadius);
bool IsPointInsideConvexHull3D(Vec3 const& point, ConvexHull3 const& convexHull3);


// get nearest point on XXX
//Vec2 const GetNearestPointOnAABB2D(Vec2 const& referencePos, AABB2 const& box);
Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referncePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine);
Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePos, Vec2 const& lineSegStart, Vec2 const& lineSegEnd);
Vec3 const GetNearestPointOnLineSegment3D(Vec3 const& referencePos, Vec3 const& lineSegStart, Vec3 const& lineSegEnd);
Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referncePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
Vec2 const GetNearestPointOnOBB2D(Vec2 const& referncePoint, OBB2 const& orientedBox);

// Normalize & De-Normalize Byte
float			NormalizeByte( unsigned char byteValue );
unsigned char	DenormalizeByte( float zeroToOne );


// text Billboard
enum class BillboardType
{
	NONE = -1,
	WORLD_UP_CAMERA_FACING,		// Rotate around the world z-axis to look at the camera position
	WORLD_DOWN_CAMERA_FACING,	// for upside down effect ( rest is same as WORLD_UP_CAMERA_FACING )
	WORLD_UP_CAMERA_OPPOSING,
	FULL_CAMERA_FACING,
	FULL_CAMERA_OPPOSING,
	COUNT
};

Mat44 GetBillboardMatrix(BillboardType billboardType, Mat44 const& cameraMatrix, Vec3 const& billboardPosition, Vec2 const& billboardScale = Vec2(1.f, 1.f));


void BounceDiscOffEachOther2D(Vec2& positionA, float radiusA, Vec2& velocityA,
							  Vec2& positionB, float radiusB, Vec2& velocityB,
							  float elasticityA = 1.f, float elasticityB = 1.f);

void BounceDiscOffFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVel,
							  Vec2 const& fixedDiscCenter, float fixedDiscRadius,
							  float mobileDiscElasticity = 1.f, float fixedDiscElasticity = 1.f);



float Lerp(float A, float B, float t);
Vec2  Lerp( Vec2 const& A, Vec2 const& B, float parametricZeroToOne );
Vec3  Lerp( Vec3 const& A, Vec3 const& B, float parametricZeroToOne );
Quaternion NormalizedLerp( Quaternion const& A, Quaternion const& B, float parametricZeroToOne );
Quaternion SphericalLerp( Quaternion const& A, Quaternion const& B, float parametricZeroToOne );
Transform LerpTransform( Transform const& A, Transform const& B, float parametricZeroToOne );

// 1D Bezier ----------------------------------------------------------------------------------------
float ComputeQuadraticBezier1D(float A, float B, float C, float t);
float ComputeCubicBezier1D(float A, float B, float C, float D, float t);
float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t);




//----------------------------------------------------------------------------------------------------------
float	   AngleBetweenQuaternions(Quaternion const& q1, Quaternion const& q2);
Quaternion RotateTowards( Quaternion const& startOrientation, Quaternion  endOrientation, float maxAngleDegrees );