#pragma once

#include "Vec2.hpp"
#include "Engine/Math/FloatRange.hpp"


class AABB2
{
public:
	Vec2 m_mins;
	Vec2 m_maxs;

public:

	// Construction / Destruction
	AABB2();
	~AABB2();
	AABB2(const AABB2& copyFrom);	// copy constructor
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(const Vec2& mins, const Vec2& maxs);

	// Accessors (const methods)
	bool IsPointInside(const Vec2& point) const;
	const Vec2 GetCenter() const;
	const Vec2 GetDimensions() const;
	const Vec2 GetNearestPoint(const Vec2& referencePosition) const;
	const Vec2 GetPointAtUV(const Vec2& uv) const;
	const Vec2 GetUVForPoint(const Vec2& point) const;
	const AABB2 GetBoxAtUVs(float uMin, float vMin, float uMax, float vMax) const;
	const AABB2 GetBoxAtUVs(AABB2 const& uvs) const;

	// Mutators (non-const methods)
	void Translate(const Vec2& translationToApply);
	void SetCenter(const Vec2& newCenter);
	void SetDimensions(const Vec2& newDimensions);
	void StretchToIncludePoint(const Vec2& point);
	void AddPaddingByUniformUV(Vec2 const& uv);

	// const operators
	bool operator==(const AABB2& other) const;

	// useful defaults
	static const AABB2 ZERO_TO_ONE;
	static const AABB2 ZERO;

	//----------------------------------------------------------------------------------------------------------
	// Modifies the AABB2 being called, and returns the chopped off bit
	AABB2 ChopOffTop( float fractionToChop, float extraAmountToChop = 0.f );
	AABB2 ChopOffBottom( float fractionToChop, float extraAmountToChop = 0.f );
	AABB2 ChopOffLeft( float fractionToChop, float extraAmountToChp = 0.f );
	AABB2 ChopOffRight( float fractionToChop, float extracAmountToiChop = 0.f );
	
	//----------------------------------------------------------------------------------------------------------
	AABB2 GetAlignedBoxWithin( Vec2 size, Vec2 alignment );
	AABB2 GetLargestBoxOfAspectWithin( float aspect, Vec2 alignment );

	//----------------------------------------------------------------------------------------------------------
	FloatRange GetXRange() const;
	FloatRange GetYRange() const;
};

