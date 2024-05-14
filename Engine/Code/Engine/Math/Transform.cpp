#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Core/EngineCommon.hpp"


//-------------------------------------------------------------------------
Transform::Transform( Vec3 const& position, Quaternion const& rotation, Vec3 const& scale )
	: m_position( position ), m_rotation( rotation ), m_scale( scale )
{
}


//-------------------------------------------------------------------------
Transform Transform::ApplyChildToParentTransform( Transform const& child, Transform const& parent )
{
	Transform result;

	// combine scale
	result.m_scale = parent.m_scale * child.m_scale;

	// combine rotation
	result.m_rotation = parent.m_rotation * child.m_rotation;

	// combine position
	result.m_position = parent.m_rotation * ( parent.m_scale * child.m_position );
	result.m_position = parent.m_position + result.m_position;

	return result;
}


//----------------------------------------------------------------------------------------------------------
Transform Transform::CreateFromMatrix( Mat44 const& matrix )
{
	UNUSED( matrix );

	// TODO: implement later
	return Transform();
}


//----------------------------------------------------------------------------------------------------------
bool Transform::operator==( Transform const& other ) const
{
	bool isPositionSame = m_position == other.m_position;
	bool isRotationSame = m_rotation == other.m_rotation;
	bool isScaleSame	= m_scale == other.m_scale;

	bool isSame = isPositionSame && isRotationSame && isScaleSame;
	return isSame;
}


//----------------------------------------------------------------------------------------------------------
bool Transform::operator!=( Transform const& other ) const
{
	bool isSame = *this == other;
	return !isSame;
}


//----------------------------------------------------------------------------------------------------------
Transform const Transform::operator-( Transform const& other ) const
{
	Transform result;
	result.m_position = m_position - other.m_position;
	result.m_rotation = m_rotation.GetDifference( other.m_rotation );
	result.m_scale	  = m_scale - other.m_scale;

	return result;
}


//----------------------------------------------------------------------------------------------------------
Transform const Transform::operator+( Transform const& other ) const
{
	Transform result;
	result.m_position = m_position + other.m_position;
	result.m_rotation = m_rotation * other.m_rotation;
	result.m_scale	  = m_scale + other.m_scale;

	return result;
}