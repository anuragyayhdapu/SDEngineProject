#pragma once


#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec4.hpp"


struct Vec3;
struct Mat44;

//-------------------------------------------------------------------------
struct Quaternion
{
	//-------------------------------------------------------------------------
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;


	//-------------------------------------------------------------------------
	Quaternion(); // Identity
	Quaternion( float initialX, float initialY, float initialZ, float initialW );


	Vec4 const GetAsVec4() const;

	//-------------------------------------------------------------------------
	static Quaternion const MakeFromAxisOfRotationAndAngleDegrees( Vec3 const& normalizedAxisOfRotation, float angleOfRotationDegrees );
	static Quaternion const MakeFromMatrix( Mat44 const& matrix );
	static Quaternion const MakeFromEulerAngles( EulerAngles const& eulerAngles );


	//-------------------------------------------------------------------------
	// operators
	Quaternion const operator+( Quaternion const& quaternionToAdd ) const;		// component wise addition between quaternions
	Quaternion const operator-( Quaternion const& quaternionToSubtract ) const; // component wise subtraction between quaternions
	Quaternion const operator-() const;											// negation
	Quaternion const operator*( float scalar ) const;							// component wise scale

	// rotation
	Quaternion const operator*( Quaternion const& rotationToAppend ) const;	// append another rotation to this rotation
	Vec3 const		 operator*( Vec3 const& vectorToRotate ) const; // rotate a vector by a quaternion

	Quaternion const GetInverse() const;
	Quaternion const GetDifference( Quaternion const& quaternionToSubtract ) const;

	//-------------------------------------------------------------------------
	// comparison
	bool operator==( Quaternion const& compare ) const;
	bool operator!=( Quaternion const& compare ) const;


	float GetLength() const;
	float GetLengthSquared() const;

	void			 Normalize();
	Quaternion const GetNormalized() const;

	//-------------------------------------------------------------------------
	// concatenation
	// TODO

	//-------------------------------------------------------------------------
	static Quaternion const IDENTITY;


	//----------------------------------------------------------------------------------------------------------
	Mat44 GetAsMatrix_XFwd_YLeft_ZUp() const;
	void  GetAsVectors_XFwd_YLeft_ZUp( Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis ) const;
	Vec3  GetVector_XFwd() const; 
	Vec3  GetVector_YLeft() const;
	Vec3  GetVector_ZUp() const;

	EulerAngles GetAsEulerAngles() const;

	//----------------------------------------------------------------------------------------------------------
	void GetAxisAndAngle( Vec3& outAxisOfRotation, float& outAngleOfRotation ) const;
};