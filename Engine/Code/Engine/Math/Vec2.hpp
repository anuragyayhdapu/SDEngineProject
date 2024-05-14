#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec2
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;

public:
	// Construction/Destruction
	~Vec2() {}												// destructor (do nothing)
	Vec2() {}												// default constructor (do nothing)
	Vec2(Vec2 const& copyFrom);							// copy constructor (from another vec2)
	explicit Vec2(float initialX, float initialY);		// explicit constructor (from x, y)

	static const Vec2 MakeFromPolarRadians(float orientationRadians, float length = 1.f);
	static const Vec2 MakeFromPolarDegrees(float orientationDegrees, float length = 1.f);

	// Accessors (const methods)
	float GetLength() const;
	float GetLengthSquared() const;
	float GetOrientationRadians() const;
	float GetOrientationDegrees() const;

	const Vec2 GetRotated90Degrees() const;
	const Vec2 GetRotatedMinus90Degrees() const;
	const Vec2 GetRotatedRadians(float deltaRadians) const;
	const Vec2 GetRotatedDegrees(float deltaDegrees) const;

	void SetOrientationRadians(float newOrientationRadians);
	void SetOrientationDegrees(float newOrientationDegrees);
	void SetPolarRadians(float newOrientationRadians, float newLength);
	void SetPolarDegrees(float newOrientationDegrees, float newLength);

	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void RotateRadians(float deltaRadians);
	void RotateDegrees(float deltaDegrees);

	const Vec2 GetClamped(float maxLength) const;
	const Vec2 GetNormalized() const;
	void SetLength(float newLength);
	void ClampLength(float maxLenght);
	void Normalize();
	float NormalizeAndGetPreviousLength();

	void Reflect(Vec2 const& reflectionPlane);
	Vec2 GetReflected(Vec2 const& reflectionPlane) const;

	// Operators (const)
	bool		operator==(const Vec2& compare) const;		// vec2 == vec2
	bool		operator!=(const Vec2& compare) const;		// vec2 != vec2
	const Vec2	operator+(const Vec2& vecToAdd) const;		// vec2 + vec2
	const Vec2	operator-(const Vec2& vecToSubtract) const;	// vec2 - vec2
	const Vec2	operator-() const;								// -vec2, i.e. "unary negation"
	const Vec2	operator*(float uniformScale) const;			// vec2 * float
	const Vec2	operator*(const Vec2& vecToMultiply) const;	// vec2 * vec2
	const Vec2	operator/(float inverseScale) const;			// vec2 / float

	// Operators (self-mutating / non-const)
	void		operator+=(const Vec2& vecToAdd);				// vec2 += vec2
	void		operator-=(const Vec2& vecToSubtract);		// vec2 -= vec2
	void		operator*=(const float uniformScale);			// vec2 *= float
	void		operator/=(const float uniformDivisor);		// vec2 /= float
	void		operator=(const Vec2& copyFrom);				// vec2 = vec2

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend const Vec2 operator*(float uniformScale, const Vec2& vecToScale);	// float * vec2

public:
	// static constants
	static const Vec2 ZERO;
	static const Vec2 ONE;

	static const Vec2 NORTH;
	static const Vec2 EAST;
	static const Vec2 SOUTH;
	static const Vec2 WEST;

	void SetFromText(char const* text);	// Parses “6,4” or “ -.3 , 0.05 ” to (x,y)
};