#pragma once
class FloatRange
{
public:
	FloatRange();
	explicit FloatRange(float min, float max);

	void operator=(const FloatRange& copyFrom);
	bool operator==(const FloatRange& compare) const;
	bool operator!=(const FloatRange& compare) const;
	bool IsOnRange(float value);
	bool IsOverlappingWith(FloatRange const& otherRange);

	float GetMin() const { return m_min; }
	float GetMax() const { return m_max; }

	void SetFromText(char const* text);

private:
	float m_min = 0.f;
	float m_max = 0.f;
};

