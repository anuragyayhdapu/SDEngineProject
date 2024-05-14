#pragma once

#include "Engine/Math/Vec2.hpp"

#include <vector>


//----------------------------------------------------------------------------------------------------------
class ConvexPolly2
{
public:
	ConvexPolly2();
	ConvexPolly2( std::vector<Vec2> const& counterClockWiseOrderedPoints );

	//----------------------------------------------------------------------------------------------------------
private:
	std::vector<Vec2> m_counterClockWiseOrderedpoints;

public:
	std::vector<Vec2> const& GetCounterClockWiseOrderedPoints() const;
	void					 AddDeltaPositionToPolyPoints(Vec2 const& deltaPosition);
};