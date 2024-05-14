#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/MathUtils.hpp"



//----------------------------------------------------------------------------------------------------------
Plane3::Plane3()
{
}


//----------------------------------------------------------------------------------------------------------
Plane3::Plane3( Vec3 const& normal, float distanceFromOrigin )
	: m_normal( normal ), m_distanceFromOrigin( distanceFromOrigin )
{
}

//----------------------------------------------------------------------------------------------------------
const Plane3 Plane3::MakeFromVectorsInPlane( Vec3 const& A, Vec3 const& B )
{
	Plane3 plane;
	plane.m_normal = CrossProduct3D( A, B );
	plane.m_normal.Normalize();

	plane.m_distanceFromOrigin = GetProjectedLength3D( A, plane.m_normal );

	return plane;
}
