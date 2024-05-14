#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <math.h>

constexpr float MARGIN_OF_ERROR = 0.000001f;


//-------------------------------------------------------------------------
Quaternion const Quaternion::IDENTITY = Quaternion();


//-------------------------------------------------------------------------
Quaternion::Quaternion()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
	w = 1.f;
}


//-------------------------------------------------------------------------
Quaternion::Quaternion( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX ), y( initialY ), z( initialZ ), w( initialW )
{
}


//-------------------------------------------------------------------------
Quaternion const Quaternion::MakeFromAxisOfRotationAndAngleDegrees( Vec3 const& normalizedAxisOfRotation, float angleOfRotationDegrees )
{
	float angleOfRotationRadins	 = ConvertDegreesToRadians( angleOfRotationDegrees );
	angleOfRotationRadins		*= 0.5f;

	float		sinAngle		 = sinf( angleOfRotationRadins );
	float		cosAngle		 = cosf( angleOfRotationRadins );
	Vec3 const& axis			 = normalizedAxisOfRotation;

	Quaternion	quaternion;
	quaternion.x = sinAngle * axis.x;
	quaternion.y = sinAngle * axis.y;
	quaternion.z = sinAngle * axis.z;
	quaternion.w = cosAngle;

	return quaternion;
}


//----------------------------------------------------------------------------------------------------------
Quaternion const Quaternion::MakeFromMatrix( Mat44 const& matrix )
{
	UNUSED( matrix );

	// TODO: implement later
	return Quaternion();
}


//----------------------------------------------------------------------------------------------------------
Quaternion const Quaternion::MakeFromEulerAngles( EulerAngles const& eulerAngles )
{
	float	   yaw		 = eulerAngles.m_yawDegrees;
	float	   pitch	 = eulerAngles.m_pitchDegrees;
	float	   roll		 = eulerAngles.m_rollDegrees;

	float	   halfYaw	 = yaw * 0.5f;
	float	   hlafPitch = pitch * 0.5f;
	float	   halfRoll	 = roll * 0.5f;

	float	   cosRoll	 = CosDegrees( halfRoll );
	float	   sinRoll	 = SinDegrees( halfRoll );
	float	   cosPitch	 = CosDegrees( hlafPitch );
	float	   sinPitch	 = SinDegrees( hlafPitch );
	float	   cosYaw	 = CosDegrees( halfYaw );
	float	   sinYaw	 = SinDegrees( halfYaw );

	Quaternion quaternion;
	quaternion.x = ( sinRoll * cosPitch * cosYaw ) - ( cosRoll * sinPitch * sinYaw );
	quaternion.y = ( cosRoll * sinPitch * cosYaw ) + ( sinRoll * cosPitch * sinYaw );
	quaternion.z = ( cosRoll * cosPitch * sinYaw ) - ( sinRoll * sinPitch * cosYaw );

	quaternion.w = ( cosRoll * cosPitch * cosYaw ) + ( sinRoll * sinPitch * sinYaw );

	return quaternion;
}


//-------------------------------------------------------------------------
Quaternion const Quaternion::operator+( Quaternion const& quaternionToAdd ) const
{
	Quaternion result;
	result.x = x + quaternionToAdd.x;
	result.y = y + quaternionToAdd.y;
	result.z = z + quaternionToAdd.z;
	result.w = w + quaternionToAdd.w;

	return result;
}


//-------------------------------------------------------------------------
Quaternion const Quaternion::operator-( Quaternion const& quaternionToSubtract ) const
{
	Quaternion result;
	result.x = x - quaternionToSubtract.x;
	result.y = y - quaternionToSubtract.y;
	result.z = z - quaternionToSubtract.z;
	result.w = w - quaternionToSubtract.w;

	return result;
}


//-------------------------------------------------------------------------
Quaternion const Quaternion::operator-() const
{
	Quaternion negatedResult;
	negatedResult.x = -x;
	negatedResult.y = -y;
	negatedResult.z = -z;
	negatedResult.w = -w;

	return negatedResult;
}


//-------------------------------------------------------------------------
Quaternion const Quaternion::operator*( float scalar ) const
{
	Quaternion scaledResult;
	scaledResult.x = x * scalar;
	scaledResult.y = y * scalar;
	scaledResult.z = z * scalar;
	scaledResult.w = w * scalar;

	return scaledResult;
}


//-------------------------------------------------------------------------
// append another rotation to 'this' rotation
// this method follow the rules of quaternion multiplication
// i.e. if result = thisQuaternion * rotationToAppend
// when using the result quaternion is used to rotate a vector, 'rotationToAppend' will be applied, then 'thisQuaternion' will be applied
Quaternion const Quaternion::operator*( Quaternion const& rotationToAppend ) const
{
	float	   realPartOfThisQuaternion		 = w;
	Vec3	   imaginaryPartofThisQuaternion = { x, y, z };

	float	   realPartOfRotor				 = rotationToAppend.w;
	Vec3	   imaginaryPartOfRotor			 = { rotationToAppend.x, rotationToAppend.y, rotationToAppend.z };

	float	   w1w2							 = realPartOfThisQuaternion * realPartOfRotor;
	float	   v1Dotv2						 = DotProduct3D( imaginaryPartofThisQuaternion, imaginaryPartOfRotor );
	Vec3	   w1v2							 = realPartOfThisQuaternion * imaginaryPartOfRotor;
	Vec3	   w2v1							 = realPartOfRotor * imaginaryPartofThisQuaternion;
	Vec3	   v1Crossv2					 = CrossProduct3D( imaginaryPartofThisQuaternion, imaginaryPartOfRotor );

	float	   scalar						 = w1w2 - v1Dotv2;
	Vec3	   vector						 = w1v2 + w2v1 + v1Crossv2;

	Quaternion result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = vector.z;
	result.w = scalar;

	return result;
}


// For reference
// https://gabormakesgames.com/blog_quats_multiply_vec.html
//-------------------------------------------------------------------------
Vec3 const Quaternion::operator*( Vec3 const& vectorToRotate ) const
{
	Vec3 vectorPartOfQuaternion = Vec3( x, y, z );

	Vec3 rotatedResult;
	rotatedResult = ( 2.f * DotProduct3D( vectorPartOfQuaternion, vectorToRotate ) * vectorPartOfQuaternion ) +
					( ( w * w - DotProduct3D( vectorPartOfQuaternion, vectorPartOfQuaternion ) ) * vectorToRotate ) +
					( 2.f * w * CrossProduct3D( vectorPartOfQuaternion, vectorToRotate ) );

	return rotatedResult;
}


//----------------------------------------------------------------------------------------------------------
Quaternion const Quaternion::GetInverse() const
{
	Quaternion const& q				= *this;
	float			  squaredLength = ( q.x * q.x ) + ( q.y * q.y ) + ( q.z * q.z ) + ( q.w * q.w );

	// not unit quaternion
	float difference = squaredLength - 1.f;
	difference		 = fabsf( difference );
	if ( difference > MARGIN_OF_ERROR )
	{
		ERROR_RECOVERABLE( "Length of Rotation Quaternion not 1. Normalize before calling Inverse." );
	}

	// unit quaternion
	Quaternion inverse{ -x, -y, -z, w };
	return inverse;
}


//----------------------------------------------------------------------------------------------------------
Quaternion const Quaternion::GetDifference( Quaternion const& quaternionToSubtract ) const
{
	Quaternion const& A		   = *this;
	Quaternion const& B		   = quaternionToSubtract;

	Quaternion const& InverseA = A.GetInverse();
	Quaternion		  result   = B * InverseA;

	return result;
}


//-------------------------------------------------------------------------
bool Quaternion::operator==( Quaternion const& compare ) const
{
	// 1. check with quaternion
	bool isEqual = ( fabsf( x - compare.x ) <= MARGIN_OF_ERROR ) &&
				   ( fabsf( y - compare.y ) <= MARGIN_OF_ERROR ) &&
				   ( fabsf( z - compare.z ) <= MARGIN_OF_ERROR ) &&
				   ( fabsf( w - compare.w ) <= MARGIN_OF_ERROR );

	// 2. check with inverse of quaternion
	bool isInverseEqual = ( fabsf( x + compare.x ) <= MARGIN_OF_ERROR ) &&
						  ( fabsf( y + compare.y ) <= MARGIN_OF_ERROR ) &&
						  ( fabsf( z + compare.z ) <= MARGIN_OF_ERROR ) &&
						  ( fabsf( w + compare.w ) <= MARGIN_OF_ERROR );


	bool isSimilarOrientation = isEqual || isInverseEqual;
	return isSimilarOrientation;
}


//-------------------------------------------------------------------------
bool Quaternion::operator!=( Quaternion const& compare ) const
{
	return !( *this == compare );
}


//-------------------------------------------------------------------------
Vec4 const Quaternion::GetAsVec4() const
{
	return Vec4( x, y, z, w );
}


//-------------------------------------------------------------------------
float Quaternion::GetLength() const
{
	float lengthSquared = ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
	float length		= sqrtf( lengthSquared );
	return length;
}


//----------------------------------------------------------------------------------------------------------
float Quaternion::GetLengthSquared() const
{
	float lengthSquared = ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
	return lengthSquared;
}


//----------------------------------------------------------------------------------------------------------
void Quaternion::Normalize()
{
	float lengthSquared = ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
	/*if ( lengthSquared < MARGIN_OF_ERROR )
	{
		return;
	}*/

	float length		 = sqrtf( lengthSquared );
	float inverseLength	 = 1.f / length;

	x					*= inverseLength;
	y					*= inverseLength;
	z					*= inverseLength;
	w					*= inverseLength;
}


//----------------------------------------------------------------------------------------------------------
Quaternion const Quaternion::GetNormalized() const
{
	float lengthSquared = ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
	/*if ( lengthSquared < MARGIN_OF_ERROR )
	{
		return Quaternion();
	}*/

	float	   length		 = sqrtf( lengthSquared );
	float	   inverseLength = 1.f / length;

	Quaternion normalizedQuaternion;
	normalizedQuaternion.x = x * inverseLength;
	normalizedQuaternion.y = y * inverseLength;
	normalizedQuaternion.z = z * inverseLength;
	normalizedQuaternion.w = w * inverseLength;

	return normalizedQuaternion;
}


//----------------------------------------------------------------------------------------------------------
Mat44 Quaternion::GetAsMatrix_XFwd_YLeft_ZUp() const
{
	Quaternion const& rotation = *this;
	Vec3			  iForward = rotation * Vec3( 1.f, 0.f, 0.f );
	Vec3			  jLeft	   = rotation * Vec3( 0.f, 1.f, 0.f );
	Vec3			  kUp	   = rotation * Vec3( 0.f, 0.f, 1.f );

	Mat44			  rotationMatrix;
	rotationMatrix.SetIJK3D( iForward, jLeft, kUp );

	return rotationMatrix;
}


//----------------------------------------------------------------------------------------------------------
void Quaternion::GetAsVectors_XFwd_YLeft_ZUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const
{
	Quaternion const& rotation = *this;
	out_forwardIBasis		   = rotation * Vec3( 1.f, 0.f, 0.f );
	out_leftJBasis			   = rotation * Vec3( 0.f, 1.f, 0.f );
	out_upKBasis			   = rotation * Vec3( 0.f, 0.f, 1.f );
}


//----------------------------------------------------------------------------------------------------------
Vec3 Quaternion::GetVector_XFwd() const
{
	Quaternion const& rotation		= *this;
	Vec3			  forwardIBasis = rotation * Vec3( 1.f, 0.f, 0.f );

	return forwardIBasis;
}


//----------------------------------------------------------------------------------------------------------
Vec3 Quaternion::GetVector_YLeft() const
{
	Quaternion const& rotation	 = *this;
	Vec3			  leftJBasis = rotation * Vec3( 0.f, 1.f, 0.f );

	return leftJBasis;
}


//----------------------------------------------------------------------------------------------------------
Vec3 Quaternion::GetVector_ZUp() const
{
	Quaternion const& rotation = *this;
	Vec3			  upKBasis = rotation * Vec3( 0.f, 0.f, 1.f );

	return upKBasis;
}


//----------------------------------------------------------------------------------------------------------
EulerAngles Quaternion::GetAsEulerAngles() const
{
	float		yawDegrees	 = Atan2Degrees( 2.f * ( w * z + x * y ), 1.f - 2.f * ( y * y + z * z ) );
	float		pitchDegrees = AsinDegrees( 2.f * ( w * y - z * x ) );
	float		rollDegrees	 = Atan2Degrees( 2.f * ( w * x + y * z ), 1.f - 2.f * ( x * x + y * y ) );

	EulerAngles eulerAngles;
	eulerAngles.m_yawDegrees   = yawDegrees;
	eulerAngles.m_pitchDegrees = pitchDegrees;
	eulerAngles.m_rollDegrees  = rollDegrees;

	return eulerAngles;
}


//----------------------------------------------------------------------------------------------------------
void Quaternion::GetAxisAndAngle( Vec3& outAxisOfRotation, float& outAngleOfRotation ) const
{
	float cosOfHalfTheta   = w;
	float halfThetaDegrees = AcosDegrees( cosOfHalfTheta );
	outAngleOfRotation	   = halfThetaDegrees * 2.f;

	float sinHalfTheta	   = SinDegrees( halfThetaDegrees );
	outAxisOfRotation.x	   = x / sinHalfTheta;
	outAxisOfRotation.y	   = y / sinHalfTheta;
	outAxisOfRotation.z	   = z / sinHalfTheta;
}
