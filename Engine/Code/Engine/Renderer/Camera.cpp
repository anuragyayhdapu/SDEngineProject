
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"


void Camera::SetOrthographicView(Vec2 const& bottomLeft, Vec2 const& topRight, float near, float far)
{
    m_mode = eMode_Orthographic;

    m_bottomLeftOrtho = bottomLeft;
    m_topRightOrtho = topRight;
    m_orthographicNear = near;
    m_orthographicFar = far;
}


void Camera::SetPerspectiveView(float aspect, float fov, float near, float far)
{
    m_mode = eMode_Perspective;

    m_perspectiveAspect = aspect;
    m_perspectiveFOV = fov;
    m_perspectiveNear = near;
    m_perspectiveFar = far;
}


void Camera::Translate2D(Vec2 const& translation2D) 
{
    m_bottomLeftOrtho += translation2D;
    m_topRightOrtho += translation2D;
}


Mat44 Camera::GetOrthographicMatrix() const
{
    float left = m_bottomLeftOrtho.x;
    float right = m_topRightOrtho.x;
    float bottom = m_bottomLeftOrtho.y;
    float top = m_topRightOrtho.y;
    float zNear = m_orthographicNear;
    float zFar = m_orthographicFar;

    Mat44 orthographicProjectionMatrix = Mat44::CreateOrthoProjection(left, right, bottom, top, zNear, zFar);
    return orthographicProjectionMatrix;
}


Mat44 Camera::GetPerspectiveMatrix() const
{
    float fovYDegrees = m_perspectiveFOV;
    float aspect = m_perspectiveAspect;
    float zNear = m_perspectiveNear;
    float zFar = m_perspectiveFar;

    Mat44 perspectiveProjectionMatrix = Mat44::CreatePerspectiveProjection(fovYDegrees, aspect, zNear, zFar);
    return perspectiveProjectionMatrix;
}


Mat44 Camera::GetProjectionMatrix() const
{
    Mat44 projectionMatrix;

    if (m_mode == eMode_Orthographic)
    {
        projectionMatrix = GetOrthographicMatrix();
    }
    else
    {
        projectionMatrix = GetPerspectiveMatrix();

		Mat44 renderMatrix = GetRenderMatrix();
		projectionMatrix.Append(renderMatrix);
    }

    return projectionMatrix;
}

void Camera::SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis)
{
    m_renderIBasis = iBasis;
    m_renderJBasis = jBasis;
    m_renderKBasis = kBasis;
}

Mat44 Camera::GetRenderMatrix() const
{
    Vec3 translate3D(0.f, 0.f, 0.f);
    Mat44 renderMatrix(m_renderIBasis, m_renderJBasis, m_renderKBasis, translate3D);

    return renderMatrix;
}


void Camera::SetTransform(Vec3 const& position, EulerAngles const& orientation)
{
    m_position = position;
	m_orientation = Quaternion::MakeFromEulerAngles( orientation );
}


//----------------------------------------------------------------------------------------------------------
void Camera::SetTransform( Vec3 const& position, Quaternion const& orientation )
{
	m_position	  = position;
	m_orientation = orientation;
}

Mat44 Camera::GetViewMatrix() const
{
    // build world to view transform matrix
    Mat44 cameraModelMatrix = m_orientation.GetAsMatrix_XFwd_YLeft_ZUp();
    cameraModelMatrix.SetTranslation3D(m_position);

    Mat44 viewMatrix = cameraModelMatrix.GetOrthonormalInverse();
    return viewMatrix;
}

void Camera::SetViewPort(AABB2 playerViewPort)
{
    m_viewPort = playerViewPort;

    if ( m_mode == eMode_Perspective )
    {
		float viewportWidth = m_viewPort.m_maxs.x - m_viewPort.m_mins.x;
		float viewportHeight = m_viewPort.m_maxs.y - m_viewPort.m_mins.y;

        float aspectRatio = viewportWidth / viewportHeight;
        m_perspectiveAspect = aspectRatio;
    }
}

float Camera::GetAspectRation() const
{
	float viewportWidth = m_viewPort.m_maxs.x - m_viewPort.m_mins.x;
	float viewportHeight = m_viewPort.m_maxs.y - m_viewPort.m_mins.y;

	float aspectRatio = viewportWidth / viewportHeight;

    return aspectRatio;
}


////----------------------------------------------------------------------------------------------------------
//void Camera::SetOrientation( EulerAngles const& orientation )
//{
//	m_orientation = orientation.GetAsMatrix_XFwd_YLeft_ZUp();
//}


//----------------------------------------------------------------------------------------------------------
void Camera::SetOrientation( Quaternion const& orientation )
{
	m_orientation = orientation;
}

//void Camera::SetOrientation( Mat44 const& orientation )
//{
//	m_orientation = orientation;
//}
