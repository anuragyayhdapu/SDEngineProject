#pragma once


class IntVec2
{
public:
	int x = 0;
	int y = 0;

public:
	// static constants 
	static const IntVec2 ZERO;

public:

	// Constructor / Destructor 
	IntVec2();
	~IntVec2();
	IntVec2(const IntVec2& copyFrom);
	explicit IntVec2(int initialX, int initialY);

	// Accessors (const methods)
	float GetLength() const;
	int GetTaxicabLength() const;
	int GetLengthSquared() const;
	float GetOrientationRadians() const;
	float GetOrientationDegrees() const;
	const IntVec2 GetRotated90Degrees() const;
	const IntVec2 GetRotatedMinus90Degrees() const;

	// Mutators (non-const methods)
	void Rotate90Degrees();
	void RotateMinus90Degrees();

	IntVec2 IncrementX();
	IntVec2 IncrementY();
	IntVec2 DecrementX();
	IntVec2 DecrementY();

	void AddX(int xToAdd);
	void AddY(int yToAdd);

	// Operators (const)
	bool operator!=(const IntVec2& compare) const;		// vec2 != vec2

	// Operators (self-mutating / non-const)
	void		  operator=(const IntVec2& copyFrom);
	IntVec2 const operator+(const IntVec2& vecToAdd) const;
	IntVec2 const operator-(const IntVec2& vecToSubtract) const;	// IntVec2 - IntVec2
	bool const operator<(const IntVec2& vecToCompare) const;
	bool const operator==(const IntVec2& compare) const;		// Intvec2 == Intvec2

	void SetFromText(char const* text);	// Parses “6,4” or “ -3 , 5 ” to (x,y)

	static int const GetManhattanDistance(IntVec2 const& a, IntVec2 const& b);
	static float const GetEuclideanDistanceSquared(IntVec2 const& a, IntVec2 const& b);
};

