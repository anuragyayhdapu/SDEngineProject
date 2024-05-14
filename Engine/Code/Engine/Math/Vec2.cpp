#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <math.h>

const Vec2 Vec2::ZERO(0.f, 0.f);
const Vec2 Vec2::ONE(1.f, 1.f);

const Vec2 Vec2::NORTH(0.f, 1.f);
const Vec2 Vec2::EAST(1.f, 0.f);
const Vec2 Vec2::SOUTH(0.f, -1.f);
const Vec2 Vec2::WEST(-1.f, 0.f);



//-----------------------------------------------------------------------------------------------
Vec2::Vec2(const Vec2& copy)
	: x(copy.x)
	, y(copy.y)
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2(float initialX, float initialY)
	: x(initialX)
	, y(initialY)
{
}


//----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length)
{
	float x = length * cosf(orientationRadians);
	float y = length * sinf(orientationRadians);

	return Vec2(x, y);
}


//----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length)
{
	float x = length * cosf(ConvertDegreesToRadians(orientationDegrees));
	float y = length * sinf(ConvertDegreesToRadians(orientationDegrees));

	return Vec2(x, y);
}


//----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	return sqrtf((x * x) + (y * y));
}


//----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	return ((x * x) + (y * y));
}


//----------------------------------------------------------------------------------------------
float Vec2::GetOrientationRadians() const
{
	return atan2f(y, x);
}


//----------------------------------------------------------------------------------------------
float Vec2::GetOrientationDegrees() const
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}


//----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotated90Degrees() const
{
	return Vec2(-y, x);
}


//----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return -1.f * GetRotated90Degrees();
}


//----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedRadians(float deltaRadians) const
{
	float thetaRadians = GetOrientationRadians();
	float newThetaRadians = thetaRadians + deltaRadians;

	return MakeFromPolarRadians(newThetaRadians, GetLength());
}


//----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedDegrees(float deltaDegrees) const
{
	return GetRotatedRadians(ConvertDegreesToRadians(deltaDegrees));
}


//----------------------------------------------------------------------------------------------
void Vec2::SetOrientationRadians(float newOrientationRadians)
{
	Vec2 rotatedVec = MakeFromPolarRadians(newOrientationRadians, GetLength());

	x = rotatedVec.x;
	y = rotatedVec.y;
}


//----------------------------------------------------------------------------------------------
void Vec2::SetOrientationDegrees(float newOrientationDegrees)
{
	SetOrientationRadians(ConvertDegreesToRadians(newOrientationDegrees));
}


//----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians(float newOrientationRadians, float newLength)
{
	Vec2 newVec = MakeFromPolarRadians(newOrientationRadians, newLength);

	x = newVec.x;
	y = newVec.y;
}


//----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength)
{
	SetPolarRadians(ConvertDegreesToRadians(newOrientationDegrees), newLength);
}


//----------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
	float oldX = x;
	x = -y;
	y = oldX;
}


//----------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
	float oldX = x;
	x = y;
	y = -oldX;
}


//----------------------------------------------------------------------------------------------
void Vec2::RotateRadians(float deltaRadians)
{
	Vec2 rotatedVec = GetRotatedRadians(deltaRadians);

	x = rotatedVec.x;
	y = rotatedVec.y;
}


//----------------------------------------------------------------------------------------------
void Vec2::RotateDegrees(float deltaDegrees)
{
	RotateRadians(ConvertDegreesToRadians(deltaDegrees));
}


//----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetClamped(float maxLength) const
{
	float length = GetLength();

	if (length <= maxLength)
		return *this;

	return (*this) * (maxLength / length);
}


//----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetNormalized() const
{
	Vec2 copy(x, y);
	copy.Normalize();

	return copy;
}


//----------------------------------------------------------------------------------------------
void Vec2::SetLength(float newLength)
{
	Normalize();
	(*this) *= newLength;
}


//----------------------------------------------------------------------------------------------
void Vec2::ClampLength(float maxLenght)
{
	(*this) = GetClamped(maxLenght);
}


//----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	float length = GetLength();
	if (length != 0.f)
	{
		(*this) /= length;
	}
}

//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{
	float oldLength = GetLength();

	Normalize();

	return oldLength;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Reflect(Vec2 const& reflectionPlane)
{
	*this = GetReflected(reflectionPlane);
}


//-----------------------------------------------------------------------------------------------
Vec2 Vec2::GetReflected(Vec2 const& reflectionPlane) const
{
	Vec2 normal = reflectionPlane;	// normal to reflection plane 
	
	float projectedLength = DotProduct2D(*this, normal);
	Vec2 projectedOntoNormal = projectedLength * normal;
	Vec2 projectedOntoTangent = (*this) - projectedOntoNormal;
	Vec2 reflection = projectedOntoTangent + (-1.f * projectedOntoNormal);

	return reflection;
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + (const Vec2& vecToAdd) const
{
	return Vec2(x + vecToAdd.x, y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-(const Vec2& vecToSubtract) const
{
	return Vec2(x - vecToSubtract.x, y - vecToSubtract.y);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2(x * -1.f, y * -1.f);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*(float uniformScale) const
{
	return Vec2(x * uniformScale, y * uniformScale);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*(const Vec2& vecToMultiply) const
{
	return Vec2(x * vecToMultiply.x, y * vecToMultiply.y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/(float inverseScale) const
{
	return Vec2(x / inverseScale, y / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=(const Vec2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=(const Vec2& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=(const Vec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*(float uniformScale, const Vec2& vecToScale)
{
	return Vec2(uniformScale * vecToScale.x, uniformScale * vecToScale.y);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==(const Vec2& compare) const
{
	return x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=(const Vec2& compare) const
{
	return !(x == compare.x && y == compare.y);
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetFromText(char const* text)
{
	Strings splitStrings = SplitStringOnDelimiter(text, ',');
	std::string xStr = splitStrings[0];
	std::string yStr = splitStrings[1];

	x = (float)atof(xStr.c_str());
	y = (float)atof(yStr.c_str());
}