#pragma once

#include "Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

/*
 * Represents a single point (vertex) of a simple 3D object intended to be rendered (drawn)
 */
class Vertex_PCUTBN
{
public:
	Vec3 m_position;
	Rgba8 m_color;
	Vec2  m_uvTexCoords;
	Vec3  m_tangent = Vec3::ZERO;
	Vec3  m_bitangent = Vec3::ZERO;
	Vec3  m_normal;

	Vertex_PCUTBN();

	// old constructor; do not use
	explicit Vertex_PCUTBN( Vec3 const& position, Vec3 const& normal, Rgba8 const& tint, Vec2 const& uvTexCoords = Vec2() );

	explicit Vertex_PCUTBN( Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords = Vec2(), Vec3 const& tangent = Vec3(), Vec3 const& bitangent = Vec3(), Vec3 const& normal = Vec3() );
};
