#pragma once

class IntVec3
{
public:
	int x = 0;
	int y = 0;
	int z = 0;

public:
	// static constants 
	static const IntVec3 ZERO;

public:

	// Constructor / Destructor 
	IntVec3();
	~IntVec3();
	IntVec3(const IntVec3& copyFrom);
	explicit IntVec3(int initialX, int initialY, int initialZ);

	// Operators (const)
	bool operator!=(const IntVec3& compare) const;    // vec3 != vec3

	// Operators (self-mutating / non-const)
	void operator=(const IntVec3& copyFrom);
	IntVec3 const operator+(const IntVec3& vecToAdd) const;
	IntVec3 const operator-(const IntVec3& vecToSubtract) const;

	void SetFromText(const char* text);
};