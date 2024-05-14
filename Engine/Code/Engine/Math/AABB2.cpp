#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"


const AABB2 AABB2::ZERO_TO_ONE = AABB2(Vec2::ZERO, Vec2::ONE);
const AABB2 AABB2::ZERO = AABB2(Vec2::ZERO, Vec2::ZERO);


AABB2::AABB2()
{
}

AABB2::~AABB2()
{
}

AABB2::AABB2(const AABB2& copyFrom)
{
	m_mins = copyFrom.m_mins;
	m_maxs = copyFrom.m_maxs;
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	m_mins.x = minX;
	m_mins.y = minY;

	m_maxs.x = maxX;
	m_maxs.y = maxY;
}

AABB2::AABB2(const Vec2& mins, const Vec2& maxs)
	: AABB2(mins.x, mins.y, maxs.x, maxs.y)
{
}

bool AABB2::IsPointInside(const Vec2& point) const
{
	if ( point.x >= m_mins.x && point.y >= m_mins.y &&
		point.x <= m_maxs.x && point.y <= m_maxs.y )
	{
		return true;
	}

	return false;
}

const Vec2 AABB2::GetCenter() const
{
	return ( m_mins + m_maxs ) / 2.f;
}

const Vec2 AABB2::GetDimensions() const
{
	return m_maxs - m_mins;
}

const Vec2 AABB2::GetNearestPoint(const Vec2& referencePosition) const
{
	Vec2 point = referencePosition;

	if ( referencePosition.x > m_maxs.x )
	{
		point.x = m_maxs.x;
	}
	if ( referencePosition.y > m_maxs.y )
	{
		point.y = m_maxs.y;
	}
	if ( referencePosition.x < m_mins.x )
	{
		point.x = m_mins.x;
	}
	if ( referencePosition.y < m_mins.y )
	{
		point.y = m_mins.y;
	}

	return point;
}

const Vec2 AABB2::GetPointAtUV(const Vec2& uv) const
{
	Vec2 dimensions = GetDimensions();
	Vec2 delta = dimensions * uv;
	Vec2 point = m_mins + delta;

	return point;
}

const Vec2 AABB2::GetUVForPoint(const Vec2& point) const
{
	Vec2 dimension = GetDimensions();
	Vec2 delta = point - m_mins;
	Vec2 uv = Vec2(delta.x / dimension.x, delta.y / dimension.y);

	return uv;
}


//----------------------------------------------------------------------------------------------------------
const AABB2 AABB2::GetBoxAtUVs( float uMin, float vMin, float uMax, float vMax ) const
{
	return GetBoxAtUVs( AABB2( uMin, vMin, uMax, vMax ) );
}


const AABB2 AABB2::GetBoxAtUVs(AABB2 const& uvs) const
{
	AABB2 boxAtUV;
	boxAtUV.m_mins = GetPointAtUV(uvs.m_mins);
	boxAtUV.m_maxs = GetPointAtUV(uvs.m_maxs);

	return boxAtUV;
}

void AABB2::Translate(const Vec2& translationToApply)
{
	m_mins += translationToApply;
	m_maxs += translationToApply;
}

void AABB2::SetCenter(const Vec2& newCenter)
{
	Vec2 oldCenter = GetCenter();
	Vec2 deltaPosition = newCenter - oldCenter;

	Translate(deltaPosition);
}

void AABB2::SetDimensions(const Vec2& newDimensions)
{
	Vec2 center = GetCenter();

	m_mins = center - ( newDimensions / 2.f );
	m_maxs = center + ( newDimensions / 2.f );
}

void AABB2::StretchToIncludePoint(const Vec2& point)
{
	if ( point.x > m_maxs.x )
	{
		m_maxs.x = point.x;
	}
	if ( point.y > m_maxs.y )
	{
		m_maxs.y = point.y;
	}
	if ( point.x < m_mins.x )
	{
		m_mins.x = point.x;
	}
	if ( point.y < m_mins.y )
	{
		m_mins.y = point.y;
	}
}

void AABB2::AddPaddingByUniformUV(Vec2 const& uv)
{
	Vec2 bottomLeftUVPadding = uv;
	Vec2 topRightUVPadding;
	topRightUVPadding.x = 1.f - uv.x;
	topRightUVPadding.y = 1.f - uv.y;

	AABB2 paddingUVs = AABB2(bottomLeftUVPadding, topRightUVPadding);
	AABB2 paddedBox = GetBoxAtUVs(paddingUVs);

	m_mins = paddedBox.m_mins;
	m_maxs = paddedBox.m_maxs;
}


bool AABB2::operator==(const AABB2& other) const
{
	return m_mins == other.m_mins && m_maxs == other.m_maxs;
}


//----------------------------------------------------------------------------------------------------------
AABB2 AABB2::ChopOffTop( float fractionToChop, float extraAmountToChop )
{
	AABB2 chopped = GetBoxAtUVs( 0.f, 1.f - fractionToChop, 1.f, 1.f );
	chopped.m_mins.y -= extraAmountToChop;

	AABB2 remaining = GetBoxAtUVs( 0.f, 0.f, 1.f, 1.f - fractionToChop );
	remaining.m_maxs.y -= extraAmountToChop;
	m_mins = remaining.m_mins;
	m_maxs = remaining.m_maxs;

	return chopped;
}


//----------------------------------------------------------------------------------------------------------
AABB2 AABB2::ChopOffBottom( float fractionToChop, float extraAmountToChop )
{
	AABB2 chopped = GetBoxAtUVs( 0.f, 0.f, 1.f, fractionToChop );
	chopped.m_maxs.y += extraAmountToChop;

	AABB2 remaining = GetBoxAtUVs( 0.f, fractionToChop, 1.f, 1.f );
	remaining.m_mins.y += extraAmountToChop;
	m_mins = remaining.m_mins;
	m_maxs = remaining.m_maxs;

	return chopped;
}


//----------------------------------------------------------------------------------------------------------
AABB2 AABB2::ChopOffLeft( float fractionToChop, float extraAmountToChop )
{
	AABB2 chopped = GetBoxAtUVs( 0.f, 0.f, fractionToChop, 1.f );
	chopped.m_maxs.x += extraAmountToChop;

	AABB2 remaining = GetBoxAtUVs( fractionToChop, 0.f, 1.f, 1.f );
	remaining.m_mins.x += extraAmountToChop;
	m_mins = remaining.m_mins;
	m_maxs = remaining.m_maxs;

	return chopped;
}


//----------------------------------------------------------------------------------------------------------
AABB2 AABB2::ChopOffRight( float fractionToChop, float extracAmountToChop )
{
	AABB2 chopped = GetBoxAtUVs( 1.f - fractionToChop, 0.f, 1.f, 1.f );
	chopped.m_mins.x -= extracAmountToChop;

	AABB2 remaining = GetBoxAtUVs( 0.f, 0.f, 1.f - fractionToChop, 1.f );
	remaining.m_maxs.x -= extracAmountToChop;
	m_mins = remaining.m_mins;
	m_maxs = remaining.m_maxs;

	return chopped;
}


//----------------------------------------------------------------------------------------------------------
AABB2 AABB2::GetAlignedBoxWithin( Vec2 fractionSize, Vec2 alignment )
{
	AABB2 fractionBox = GetBoxAtUVs( 0.f, 0.f, fractionSize.x, fractionSize.y );
	float deltaX	  = m_maxs.x - fractionBox.m_maxs.x;
	float deltaY	  = m_maxs.y - fractionBox.m_maxs.y;
	
	AABB2 alignedBox;
	alignedBox.m_mins.x = fractionBox.m_mins.x + ( deltaX * alignment.x );
	alignedBox.m_maxs.x = fractionBox.m_maxs.x + ( deltaX * alignment.x );
	alignedBox.m_mins.y = fractionBox.m_mins.y + ( deltaY * alignment.y );
	alignedBox.m_maxs.y = fractionBox.m_maxs.y + ( deltaY * alignment.y );

	return alignedBox;
}


//----------------------------------------------------------------------------------------------------------
AABB2 AABB2::GetLargestBoxOfAspectWithin( float aspect, Vec2 alignment )
{
	UNUSED( aspect );
	UNUSED( alignment );

	return AABB2();
}


//----------------------------------------------------------------------------------------------------------
FloatRange AABB2::GetXRange() const
{
	return FloatRange( m_mins.x, m_maxs.x );
}


//----------------------------------------------------------------------------------------------------------
FloatRange AABB2::GetYRange() const
{
	return FloatRange( m_mins.y, m_maxs.y );
}
