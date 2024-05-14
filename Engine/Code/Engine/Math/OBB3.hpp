#pragma once

#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vec3.hpp"


//----------------------------------------------------------------------------------------------------------
class OBB3
{
public:
	explicit OBB3();
	explicit OBB3( Vec3 const& center, Vec3 const& iBasisNormal, Vec3 const& jBasisNormal, Vec3 dimensions );
	explicit OBB3( Vec3 const& center, Quaternion const& orientation, Vec3 dimensions );

	//----------------------------------------------------------------------------------------------------------
	Vec3 m_center;
	Vec3 m_iBasisNormal;
	Vec3 m_jBasisNormal;
	Vec3 m_dimensions;
};