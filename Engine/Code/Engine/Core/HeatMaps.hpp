#pragma once

#include "Engine/Math/IntVec2.hpp"

#include <vector>

class TileHeatMap
{
public:
	TileHeatMap(IntVec2 const& dimensions);

	float GetValue(int index) const;
	float GetValue(IntVec2 const& tileCoords) const;
	float GetValue(int tileX, int tileY) const;

	void SetAllValues(float newValue);
	void SetValue(int index, float newValue);

	int GetSize() const;

	int GetLowestValueNeighbor(int index) const;
	int GetLowestValueNeighborForHexMap( int index ) const;

private:
	IntVec2 const		m_dimensions;
	std::vector<float>	m_values;	// Stores general-purpose “heat” float values in a 1D dynamic array, one per tile in dimensions

	void InitValues();
};