#pragma once

struct Vec4
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	Vec4();
	explicit Vec4(float x, float y, float z, float w);

	
	Vec4 operator-(Vec4 const& vecToSubtract) const;
	void operator*=(float uniformScale);

	//----------------------------------------------------------------------------------------------------------
	void SetFromText( char const* text );
};