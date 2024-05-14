#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"


//----------------------------------------------------------------------------------------------------------
// Similar to Vertex_PCUTBN but with additional data to allow skinning
class Vertex_Skeletal
{
public:
	Vec3  m_position	= Vec3::ZERO;
	Rgba8 m_color		= Rgba8::WHITE;
	Vec2  m_uvTexCoords = Vec2::ZERO;
	Vec3  m_tangent		= Vec3::ZERO;
	Vec3  m_bitangent	= Vec3::ZERO;
	Vec3  m_normal		= Vec3::ZERO;
	int	  m_jointId		= 0;

	explicit Vertex_Skeletal();
	explicit Vertex_Skeletal( Vec3 const& position, Rgba8 color, Vec2 uvTexCoords, Vec3 const& tangent, Vec3 const& biTangent, Vec3 const& normal, int jointId );
};