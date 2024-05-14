#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <math.h>


const IntVec2 IntVec2::ZERO(0, 0);


IntVec2::IntVec2()
{
}

IntVec2::~IntVec2()
{
}

IntVec2::IntVec2(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

IntVec2::IntVec2(int initialX, int initialY)
{
	x = initialX;
	y = initialY;
}

float IntVec2::GetLength() const
{
	return sqrtf(static_cast<float>(GetLengthSquared()));
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

int IntVec2::GetLengthSquared() const
{
	return (x * x) + (y * y);
}

float IntVec2::GetOrientationRadians() const
{
	return atan2f(static_cast<float>(y), static_cast<float>(x));
}

float IntVec2::GetOrientationDegrees() const
{
	return GetOrientationRadians() * (180.f / M_PI);
}

const IntVec2 IntVec2::GetRotated90Degrees() const
{
	return IntVec2(-y, x);
}

const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2(y, -x);
}

void IntVec2::Rotate90Degrees()
{
	IntVec2 rotatedIntVec2 = GetRotated90Degrees();

	x = rotatedIntVec2.x;
	y = rotatedIntVec2.y;
}

void IntVec2::RotateMinus90Degrees()
{
	IntVec2 rotatedIntVec2 = GetRotatedMinus90Degrees();

	x = rotatedIntVec2.x;
	y = rotatedIntVec2.y;
}

IntVec2 IntVec2::IncrementX()
{
	IntVec2 incrementedIntVec2 = IntVec2(x + 1, y);
	return incrementedIntVec2;
}

IntVec2 IntVec2::IncrementY()
{
	IntVec2 incrementedIntVec2 = IntVec2(x, y + 1);
	return incrementedIntVec2;
}

IntVec2 IntVec2::DecrementX()
{
	IntVec2 decrementedIntVec2 = IntVec2(x - 1, y);
	return decrementedIntVec2;
}

IntVec2 IntVec2::DecrementY()
{
	IntVec2 decrementedIntVec2 = IntVec2(x, y - 1);
	return decrementedIntVec2;
}

void IntVec2::AddX(int xToAdd)
{
	x += xToAdd;
}

void IntVec2::AddY(int yToAdd)
{
	y += yToAdd;
}

void IntVec2::operator=(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

bool IntVec2::operator!=(const IntVec2& compare) const
{
	if ( ( x == compare.x ) && ( y == compare.y ) ) // x and y are same, hence not equal is false
	{
		return false;
	}
	else // x and y are different, hence not equal is true
	{
		return true;
	}
}

 IntVec2 const IntVec2::operator+(const IntVec2& vecToAdd) const
{
	 return IntVec2(x + vecToAdd.x, y + vecToAdd.y);
}

 const IntVec2 IntVec2::operator-(const IntVec2& vecToSubtract) const
 {
	 return IntVec2(x - vecToSubtract.x, y - vecToSubtract.y);
 }

 bool const IntVec2::operator<(const IntVec2& vecToCompare) const
 {
	 if (y < vecToCompare.y)
	 {
		 return true;
	 }
	 else if (y > vecToCompare.y)
	 {
		 return false;
	 }
	 else // y == vecToCompare.y
	 {
		 if (x < vecToCompare.x)
		 {
			 return true;
		 }
		 else if (x > vecToCompare.x)
		 {
			 return false;
		 }
	 }

	 return false; // equal
 }

 bool const IntVec2::operator==(const IntVec2& compare) const
 {
	 bool areEqual = (x == compare.x) && (y == compare.y);
	return areEqual;
 }

 void IntVec2::SetFromText(char const* text)
 {
	 Strings splitStrings = SplitStringOnDelimiter(text, ',');
	 std::string xStr = splitStrings[0];
	 std::string yStr = splitStrings[1];

	 x = atoi(xStr.c_str());
	 y = atoi(yStr.c_str());
 }


 int const IntVec2::GetManhattanDistance(IntVec2 const& a, IntVec2 const& b)
 {
	 int xDiff = abs(a.x - b.x);
	 int yDiff = abs(a.y - b.y);
	 int manhattanDistance = xDiff + yDiff;

	 return manhattanDistance;
 }

 float const IntVec2::GetEuclideanDistanceSquared(IntVec2 const& a, IntVec2 const& b)
 {
	 int xDiff = abs(a.x - b.x);
	 int yDiff = abs(a.y - b.y);
	 int euclideanDistanceSquared = (xDiff * xDiff) + (yDiff * yDiff);

	 return static_cast<float>(euclideanDistanceSquared);
 }
