#pragma once

#include "Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

/*
 * Represents a single point (vertex) of a simple 3D object intended to be rendered (drawn)
 */
class Vertex_PCU
{
public:
	Vec3  m_position;
	Rgba8 m_color;
	Vec2  m_uvTexCoords;

	Vertex_PCU();
	explicit Vertex_PCU( Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords = Vec2() );
};
