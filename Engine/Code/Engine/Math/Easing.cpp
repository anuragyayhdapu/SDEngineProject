#include "Engine/Math/Easing.hpp"
#include "Engine/Math/MathUtils.hpp"


float Identity(float t)
{
	float P = t;

	return P;
}


//----------------------------------------------------------------------------------------------------------
float InverseIdentity( float t )
{
	float P = -t + 1.f;

	return P;
}


float SmoothStart2(float t)
{
	float P = t * t;

	return P;
}

float SmoothStart3(float t)
{
	float P = t * t * t;

	return P;
}

float SmoothStart4(float t)
{
	float P = t * t * t * t;

	return P;
}

float SmoothStart5(float t)
{
	float P = t * t * t * t * t;

	return P;
}

float SmoothStart6(float t)
{
	float P = t * t * t * t * t * t;

	return P;
}

float SmoothStop2(float t)
{
	float s = ( 1.f - t );
	float p = 1.f - ( s * s );

	return p;
}

float SmoothStop3(float t)
{
	float s = ( 1.f - t );
	float p = 1.f - ( s * s * s);

	return p;
}

float SmoothStop4(float t)
{
	float s = ( 1.f - t );
	float p = 1.f - ( s * s * s * s);

	return p;
}

float SmoothStop5(float t)
{
	float s = ( 1.f - t );
	float p = 1.f - ( s * s * s * s * s);

	return p;
}

float SmoothStop6(float t)
{
	float s = ( 1.f - t );
	float p = 1.f - ( s * s * s * s * s * s);

	return p;
}

float SmoothStep3(float t)
{
	return (3.f * t * t) - (2.f * t * t * t);
}

float SmoothStep5(float t)
{
	float p = 0.f;
	p = ComputeQuinticBezier1D(0.f, 0.f, 0.f, 1.f, 1.f, 1.f, t);

	return p;
}

float Hesitate3(float t)
{
	float p = 0.f;
	p = ComputeCubicBezier1D(0.f, 1.f, 0.f, 1.f, t);

	return p;
}

float Hesitate5(float t)
{
	float p = 0.f;
	p = ComputeQuinticBezier1D(0.f, 1.f, 0.f, 1.f, 0.f, 1.f, t);

	return p;
}

float CustomFunkyEasing(float t)
{
	float p = 0.f;
	p = ComputeQuinticBezier1D(1.f, 0.f, 1.f, 1.f, 0.f, 0.f, t);

	return p;
}
