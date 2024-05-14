#pragma once

#include "Engine/Math/ConvexPolly2.hpp"
#include "Engine/Math/Plane2.hpp"


#include <vector>


//----------------------------------------------------------------------------------------------------------
class ConvexHull2
{
public:
	explicit ConvexHull2();
	explicit ConvexHull2(ConvexPolly2 const& convexPolly2);

	std::vector<Plane2> m_boundingPlanes;
};