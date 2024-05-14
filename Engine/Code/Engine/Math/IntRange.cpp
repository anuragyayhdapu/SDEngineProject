#include "IntRange.hpp"

IntRange::IntRange()
{
	m_min = 0;
	m_max = 0;
}

IntRange::IntRange(int min, int max)
{
	m_min = min;
	m_max = max;
}

void IntRange::operator=(const IntRange& copyFrom)
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}

bool IntRange::operator==(const IntRange& compare) const
{
	return (m_min == compare.m_min && m_max == compare.m_max);
}

bool IntRange::operator!=(const IntRange& compare) const
{
	return !(m_min == compare.m_min && m_max == compare.m_max);
}

bool IntRange::IsOnRange(int value)
{
	if (value >= m_min && value <= m_max)
		return true;
	else
		return false;
}

bool IntRange::IsOverlappingWith(IntRange const& otherRange)
{
	if (otherRange.m_min < m_min && otherRange.m_max < m_min)
		return false;
	if (otherRange.m_min > m_max && otherRange.m_max > m_max)
		return false;

	return true;
}
