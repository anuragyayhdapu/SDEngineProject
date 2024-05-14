#pragma once

#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vec3.hpp"

struct Mat44;


//-------------------------------------------------------------------------
struct Transform
{
	Vec3	   m_position = Vec3::ZERO;
	Quaternion m_rotation = Quaternion::IDENTITY;
	Vec3	   m_scale	  = Vec3( 1.f, 1.f, 1.f );


	Transform() {}
	Transform( Vec3 const& position, Quaternion const& rotation, Vec3 const& scale );
	static Transform ApplyChildToParentTransform( Transform const& child, Transform const& parent );
	static Transform CreateFromMatrix( Mat44 const& matrix );

	bool			 operator==( Transform const& other ) const;
	bool			 operator!=( Transform const& other ) const;
	Transform const	 operator-( Transform const& other ) const; // transform - transform
	Transform const	 operator+( Transform const& other ) const; // transform + transform
};