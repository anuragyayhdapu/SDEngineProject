#include "ConvexPolly2.hpp"


//----------------------------------------------------------------------------------------------------------
ConvexPolly2::ConvexPolly2()
{

}



//----------------------------------------------------------------------------------------------------------
ConvexPolly2::ConvexPolly2( std::vector<Vec2> const& counterClockWiseOrderedPoints )
	: m_counterClockWiseOrderedpoints( counterClockWiseOrderedPoints )
{
}


//----------------------------------------------------------------------------------------------------------
std::vector<Vec2> const& ConvexPolly2::GetCounterClockWiseOrderedPoints() const
{
	return m_counterClockWiseOrderedpoints;
}


//----------------------------------------------------------------------------------------------------------
void ConvexPolly2::AddDeltaPositionToPolyPoints( Vec2 const& deltaPosition )
{
	for ( int index = 0; index < m_counterClockWiseOrderedpoints.size(); index++ )
	{
		Vec2& point = m_counterClockWiseOrderedpoints[index];
		point += deltaPosition;
	}
}
