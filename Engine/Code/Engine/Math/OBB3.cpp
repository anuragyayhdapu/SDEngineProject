#include "Engine/Math/OBB3.hpp"


//----------------------------------------------------------------------------------------------------------
OBB3::OBB3()
{
}


//----------------------------------------------------------------------------------------------------------
OBB3::OBB3( Vec3 const& center, Vec3 const& iBasisNormal, Vec3 const& jBasisNormal, Vec3 dimensions )
	: m_center( center ), m_iBasisNormal( iBasisNormal ), m_jBasisNormal( jBasisNormal ), m_dimensions( dimensions )
{
}


//----------------------------------------------------------------------------------------------------------
OBB3::OBB3( Vec3 const& center, Quaternion const& orientation, Vec3 dimensions )
	: m_center( center ), m_dimensions( dimensions )
{
	Vec3 kBasisNormal;

	orientation.GetAsVectors_XFwd_YLeft_ZUp( m_iBasisNormal, m_jBasisNormal, kBasisNormal );
	m_iBasisNormal.Normalize();
	m_jBasisNormal.Normalize();
}
