#include "FloatRange.hpp"

#include "Engine/Core/StringUtils.hpp"

FloatRange::FloatRange()
{
	m_min = 0.f;
	m_max = 0.f;
}

FloatRange::FloatRange(float min, float max)
{
	m_min = min;
	m_max = max;
}

void FloatRange::operator=(const FloatRange& copyFrom)
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

bool FloatRange::operator==(const FloatRange& compare) const
{
	return (m_min == compare.m_min && m_max == compare.m_max);
}

bool FloatRange::operator!=(const FloatRange& compare) const
{
	return !(m_min == compare.m_min && m_max == compare.m_max);
}

bool FloatRange::IsOnRange(float value)
{
	if (value >= m_min && value <= m_max)
		return true;
	else
		return false;
}

bool FloatRange::IsOverlappingWith(FloatRange const& otherRange)
{
	if (otherRange.m_min < m_min && otherRange.m_max < m_min)
		return false;
	if (otherRange.m_min > m_max && otherRange.m_max > m_max)
		return false;

	return true;
}

void FloatRange::SetFromText(char const* text)
{
	Strings splitStrings = SplitStringOnDelimiter(text, '~');
	std::string minStr = splitStrings[ 0 ];
	std::string maxStr = splitStrings[ 1 ];

	m_min = ( float ) atof(minStr.c_str());
	m_max = ( float ) atof(maxStr.c_str());
}
