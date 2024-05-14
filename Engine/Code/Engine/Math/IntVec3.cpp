#include "Engine/Math/IntVec3.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <math.h>


const IntVec3 IntVec3::ZERO(0, 0, 0);

IntVec3::IntVec3() {}

IntVec3::~IntVec3() {}

IntVec3::IntVec3(const IntVec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

IntVec3::IntVec3(int initialX, int initialY, int initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

void IntVec3::operator=(const IntVec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

bool IntVec3::operator!=(const IntVec3& compare) const
{
	return (x != compare.x || y != compare.y || z != compare.z);
}

IntVec3 const IntVec3::operator+(const IntVec3& vecToAdd) const
{
	return IntVec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

IntVec3 const IntVec3::operator-(const IntVec3& vecToSubtract) const
{
	return IntVec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

void IntVec3::SetFromText(const char* text)
{
	Strings splitStrings = SplitStringOnDelimiter(text, ',');
	x = atoi(splitStrings[0].c_str());
	y = atoi(splitStrings[1].c_str());
	z = atoi(splitStrings[2].c_str());
}