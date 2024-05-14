#pragma once

#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"

class Camera
{
public:
	// Mode Enum -------------------------------------------------------------------------------------
#pragma warning( push )
#pragma warning( disable : 26812 )
	enum Mode
	{
		eMode_Orthographic,
		eMode_Perspective,

		eMode_Count
	};


	Mode GetMode() const { return m_mode; }
#pragma warning( pop )

	// Projection Matrix -----------------------------------------------------------------------------
	void SetOrthographicView( Vec2 const& bottomLeft, Vec2 const& topRight, float near = 0.f, float far = 1.f );
	void SetPerspectiveView( float aspect, float fov, float near, float far );

	Vec2  GetOrthographicBottomLeft() const { return m_bottomLeftOrtho; }
	Vec2  GetOrthographicTopRight() const { return m_topRightOrtho; }
	AABB2 GetOrthographicBounds() const { return AABB2( m_bottomLeftOrtho, m_topRightOrtho ); }
	void  Translate2D( Vec2 const& translation2D );

	Mat44 GetOrthographicMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

	// Render Matrix ---------------------------------------------------------------------------------
	void  SetRenderBasis( Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis );
	Mat44 GetRenderMatrix() const;

	// View Matrix -----------------------------------------------------------------------------------
	void  SetTransform( Vec3 const& position, EulerAngles const& orientation );
	void  SetTransform( Vec3 const& position, Quaternion const& orientation );
	Mat44 GetViewMatrix() const;
	Quaternion GetOrientation() const { return m_orientation; }

	float m_perspectiveFOV;

	void  SetViewPort( AABB2 playerViewPort );
	AABB2 m_viewPort;
	float GetAspectRation() const;

public:
	// Projection Settings ---------------------------------------------------------------------------
	Mode m_mode = eMode_Orthographic;

	Vec2  m_bottomLeftOrtho;
	Vec2  m_topRightOrtho;
	float m_orthographicNear;
	float m_orthographicFar;


	float m_perspectiveAspect;
	float m_perspectiveNear;
	float m_perspectiveFar;

	// Render Settings -------------------------------------------------------------------------------
	Vec3 m_renderIBasis = Vec3( 1.f, 0.f, 0.f );
	Vec3 m_renderJBasis = Vec3( 0.f, 1.f, 0.f );
	Vec3 m_renderKBasis = Vec3( 0.f, 0.f, 1.f );

	// View Settings ---------------------------------------------------------------------------------
	Vec3 m_position;

	//void SetOrientation( EulerAngles const& orientation );
	void SetOrientation( Quaternion const& orientation );
	//void SetOrientation( Mat44 const& orientation );

protected:
	/*Quaternion	m_quaternionOrientation;
	EulerAngles m_orientation;*/
	Quaternion m_orientation = Quaternion::IDENTITY;
};
