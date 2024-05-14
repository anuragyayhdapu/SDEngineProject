#include "Engine/Core/HeatMaps.hpp"


TileHeatMap::TileHeatMap(IntVec2 const& dimensions)
	: m_dimensions(dimensions)
{
	InitValues();
}

void TileHeatMap::InitValues()
{
	int heatMapSize = m_dimensions.x * m_dimensions.y;
	m_values.resize(heatMapSize);
}

void TileHeatMap::SetAllValues(float newValue)
{
	m_values.clear();

	int heatMapSize = m_dimensions.x * m_dimensions.y;
	m_values.resize(heatMapSize, newValue);
}

float TileHeatMap::GetValue(int index) const
{
	float value = m_values[index];
	return value;
}

float TileHeatMap::GetValue(IntVec2 const& tileCoords) const
{
	int index = tileCoords.x + (tileCoords.y * m_dimensions.x);
	float value = m_values[index];
	return value;
}

float TileHeatMap::GetValue(int tileX, int tileY) const
{
	int index = tileX + (tileY * m_dimensions.x);
	float value = m_values[index];
	return value;
}

void TileHeatMap::SetValue(int index, float newValue)
{
	m_values[index] = newValue;
}

int TileHeatMap::GetSize() const
{
	return m_dimensions.x * m_dimensions.y;
}

int TileHeatMap::GetLowestValueNeighbor(int index) const
{
	float lowestValue = m_values[index];
	int lowestValueNeighbor = index;

	int easttNeighbor = index + 1;
	int westNeighbor = index - 1;
	int northNeighbor = index + m_dimensions.x;
	int southNeighbor = index - m_dimensions.x;

	if (easttNeighbor < GetSize() && m_values[easttNeighbor] < lowestValue)
	{
		lowestValue = m_values[easttNeighbor];
		lowestValueNeighbor = easttNeighbor;
	}

	if (westNeighbor >= 0 && m_values[westNeighbor] < lowestValue)
	{
		lowestValue = m_values[westNeighbor];
		lowestValueNeighbor = westNeighbor;
	}

	if (northNeighbor < GetSize() && m_values[northNeighbor] < lowestValue)
	{
		lowestValue = m_values[northNeighbor];
		lowestValueNeighbor = northNeighbor;
	}

	if (southNeighbor >= 0 && m_values[southNeighbor] < lowestValue)
	{
		lowestValue = m_values[southNeighbor];
		lowestValueNeighbor = southNeighbor;
	}

	return lowestValueNeighbor;
}


//----------------------------------------------------------------------------------------------------------
int TileHeatMap::GetLowestValueNeighborForHexMap( int index ) const
{
	float lowestValue = m_values[index];
	int lowestValueNeighbor = index;

	int eastNeighbor = index + 1;
	int westNeighbor = index - 1;
	int northNeighbor = index + m_dimensions.x;
	int southNeighbor = index - m_dimensions.x;

	if (eastNeighbor < GetSize() && m_values[eastNeighbor] < lowestValue)
	{
		lowestValue = m_values[eastNeighbor];
		lowestValueNeighbor = eastNeighbor;
	}

	if (westNeighbor >= 0 && m_values[westNeighbor] < lowestValue)
	{
		lowestValue = m_values[westNeighbor];
		lowestValueNeighbor = westNeighbor;
	}

	if (northNeighbor < GetSize() && m_values[northNeighbor] < lowestValue)
	{
		lowestValue = m_values[northNeighbor];
		lowestValueNeighbor = northNeighbor;
	}

	if (southNeighbor >= 0 && m_values[southNeighbor] < lowestValue)
	{
		lowestValue = m_values[southNeighbor];
		lowestValueNeighbor = southNeighbor;
	}

	int east2Neighbor = index - m_dimensions.x + 1;
	int west2Neighbor = index + m_dimensions.x - 1;

	if (east2Neighbor < GetSize() && east2Neighbor >= 0 && m_values[east2Neighbor] < lowestValue)
	{
		lowestValue = m_values[east2Neighbor];
		lowestValueNeighbor = east2Neighbor;
	}

	if (west2Neighbor >= 0 && west2Neighbor < GetSize() && m_values[west2Neighbor] < lowestValue)
	{
		lowestValue = m_values[west2Neighbor];
		lowestValueNeighbor = west2Neighbor;
	}

	return lowestValueNeighbor;
}
