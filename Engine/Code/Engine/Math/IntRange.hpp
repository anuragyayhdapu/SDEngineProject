#pragma once
class IntRange
{
public:
	IntRange();
	explicit IntRange(int min, int max);

	void operator=(const IntRange& copyFrom);
	bool operator==(const IntRange& compare) const;
	bool operator!=(const IntRange& compare) const;
	bool IsOnRange(int value);
	bool IsOverlappingWith(IntRange const& otherRange);

private:
	int m_min = 0;
	int m_max = 0;
};

