#pragma once

#include "Vec2.hpp"

/*
* Like Vec2, but with a 3D “z” component added
*/
struct Vec3
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vec3();
	Vec3(float x, float y, float z);
	explicit Vec3(const Vec2& vec2);
	explicit Vec3(const Vec2& vec2, float z);

	static Vec3 const MakeFromPolarRadians(float latitudeRadians, float longitudeRadians, float length = 1.f);
	static Vec3 const MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length = 1.f);

	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutZRadians() const;
	float GetAngleAboutZDegrees() const;

	const Vec3 GetRotatedAboutZRadians(float deltaRadians) const;
	const Vec3 GetRotatedAboutZDegrees(float deltaDegrees) const;

	void SetScaleXY(float uniformScale);
	// Bug: bug in rotate about z radians
	void RotateAboutZRadians(float deltaRadians);

	const Vec3 operator+(const Vec3& vecToAdd) const;			// vec3 + vec3
	const Vec3 operator-(const Vec3& vecToSubtract) const;		// vec3 - vec3
	const Vec3 operator*(float uniformScale) const;				// vec3 * scale
	const Vec3 operator*(const Vec3& vecToMultiply) const;		// vec3 * vec3
	const Vec3 operator/(float uniformScale) const;				// vec3 / float
	bool operator==(const Vec3& vecToCompare) const;			// vec3 == vec3
	bool operator!=(const Vec3& vecToCompare) const;			// vec3 != vec3

	void operator +=(const Vec3& vecToAdd);						// vec3 += vec3
	void operator +=(const Vec2& vecToAdd);						// vec3 += vec2
	void operator -=(const Vec3& vecToSubtract);				// vec3 -= vec3
	void operator *=(float uniformScale);						// vec3 *= float
	void operator /=(float uniformScale);						// vec3 /= float


	friend const Vec3 operator*(float uniformScale, const Vec3& vecToMultiply);	// float * vec3

	void Normalize();
	Vec3 const GetNormalized() const;

	void SetFromText(char const* text);

public:
	static Vec3 const ZERO;
};

