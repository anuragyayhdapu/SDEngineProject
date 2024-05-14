#pragma once

#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Plane3.hpp"


#include <vector>


//----------------------------------------------------------------------------------------------------------
class ConvexHull3
{
public:
	explicit ConvexHull3();
	explicit ConvexHull3( AABB3 const& aabb3 );

	std::vector<Plane3> m_boundingPlanes;
};