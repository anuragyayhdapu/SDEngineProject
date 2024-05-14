#include "Vec3.hpp"
#include "MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <math.h>


// static consts
Vec3 const Vec3::ZERO = Vec3(0.f, 0.f, 0.f);


Vec3::Vec3() :
	x(0.f), y(0.f), z(0.f)
{
}

Vec3::Vec3(float x, float y, float z) :
	x(x), y(y), z(z)
{
}

Vec3::Vec3(const Vec2& vec2) :
	x(vec2.x), y(vec2.y), z(0.f)
{
}

Vec3::Vec3(const Vec2& vec2, float z) :
	x(vec2.x), y(vec2.y), z(z)
{
}


Vec3 const Vec3::MakeFromPolarRadians(float latitudeRadians, float longitudeRadians, float length)
{
	float x = length * cosf(latitudeRadians) * cosf(longitudeRadians);
	float y = length * cosf(latitudeRadians) * sinf(longitudeRadians);
	float z = length * sinf(latitudeRadians);

	return Vec3(x, y, z);
}


// Latitude goes flat, longitude goes long
Vec3 const Vec3::MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length)
{
	return MakeFromPolarRadians(ConvertDegreesToRadians(latitudeDegrees), ConvertDegreesToRadians(longitudeDegrees), length);
}


float Vec3::GetLength() const
{
	return sqrtf(GetLengthSquared());
}

float Vec3::GetLengthXY() const
{
	return sqrtf(GetLengthXYSquared());
}

float Vec3::GetLengthSquared() const
{
	return (x * x) + (y * y) + (z * z);
}

float Vec3::GetLengthXYSquared() const
{
	return (x * x) + (y * y);
}

float Vec3::GetAngleAboutZRadians() const
{
	return atan2f(y, x);
}

float Vec3::GetAngleAboutZDegrees() const
{
	return ConvertRadiansToDegrees(GetAngleAboutZRadians());
}

const Vec3 Vec3::GetRotatedAboutZRadians(float deltaRadians) const
{
	float thetaRadians = GetAngleAboutZRadians();
	float newThetaRadians = thetaRadians + deltaRadians;
	float length = GetLengthXY();
	float tempX = length * cosf(newThetaRadians);
	float tempY = length * sinf(newThetaRadians);

	return Vec3(tempX, tempY, z);
}

const Vec3 Vec3::GetRotatedAboutZDegrees(float deltaDegrees) const
{
	return GetRotatedAboutZRadians(ConvertDegreesToRadians(deltaDegrees));
}

void Vec3::SetScaleXY(float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
}

void Vec3::RotateAboutZRadians(float deltaRadians)
{
	*this = GetRotatedAboutZRadians(deltaRadians);
}

const Vec3 Vec3::operator+(const Vec3& vecToAdd) const
{
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

const Vec3 Vec3::operator-(const Vec3& vecToSubtract) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

const Vec3 Vec3::operator*(float uniformScale) const
{
	return Vec3(x * uniformScale, y * uniformScale, z * uniformScale);
}

const Vec3 Vec3::operator*(const Vec3& vecToMultiply) const
{
	return Vec3(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z);
}

const Vec3 Vec3::operator/(float uniformScale) const
{
	return Vec3(x / uniformScale, y / uniformScale, z / uniformScale);
}

bool Vec3::operator==(const Vec3& vecToCompare) const
{
	return x == vecToCompare.x && y == vecToCompare.y && z == vecToCompare.z;
}

bool Vec3::operator!=(const Vec3& vecToCompare) const
{
	return !(*this == vecToCompare);
}

void Vec3::operator+=(const Vec3& vecToAdd)
{
	(*this) = (*this) + vecToAdd;
}

void Vec3::operator+=(const Vec2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

void Vec3::operator-=(const Vec3& vecToSubtract)
{
	(*this) = (*this) - vecToSubtract;
}

void Vec3::operator*=(float uniformScale)
{
	(*this) = (*this) * uniformScale;
}

void Vec3::operator/=(float uniformScale)
{
	(*this) = (*this) / uniformScale;
}

const Vec3 operator*(float uniformScale, const Vec3& vecToMultiply)
{
	return vecToMultiply * uniformScale;
}

void Vec3::Normalize()
{
	float length = GetLength();
	x /= length;
	y /= length;
	z /= length;
}

Vec3 const Vec3::GetNormalized() const
{
	Vec3 normalizdVector = *this;
	normalizdVector.Normalize();
	 
	return normalizdVector;
}

void Vec3::SetFromText(char const* text)
{
	Strings splitStrings = SplitStringOnDelimiter(text, ',');
	std::string xStr = splitStrings[ 0 ];
	std::string yStr = splitStrings[ 1 ];
	std::string zStr = splitStrings[ 2 ];

	x = ( float ) atof(xStr.c_str());
	y = ( float ) atof(yStr.c_str());
	z = ( float ) atof(zStr.c_str());
}