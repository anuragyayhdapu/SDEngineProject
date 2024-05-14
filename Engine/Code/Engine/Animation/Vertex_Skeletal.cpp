#include "Engine/Animation/Vertex_Skeletal.hpp"


//----------------------------------------------------------------------------------------------------------
Vertex_Skeletal::Vertex_Skeletal()
{
}


//----------------------------------------------------------------------------------------------------------
Vertex_Skeletal::Vertex_Skeletal( Vec3 const& position, Rgba8 color, Vec2 uvTexCoords, Vec3 const& tangent, Vec3 const& biTangent, Vec3 const& normal, int jointId )
	: m_position( position ), m_color( color ), m_uvTexCoords( uvTexCoords ), m_tangent( tangent ), m_bitangent( biTangent ), m_normal( normal ), m_jointId( jointId )
{
}
