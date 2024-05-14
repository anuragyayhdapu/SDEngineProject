#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

Mat44 const Mat44::IDENTITY = Mat44();

Mat44::Mat44()
{
	m_values[Ix] = 1.f;  m_values[Jx] = 0.f;  m_values[Kx] = 0.f;  m_values[Tx] = 0.f;
	m_values[Iy] = 0.f;  m_values[Jy] = 1.f;  m_values[Ky] = 0.f;  m_values[Ty] = 0.f;
	m_values[Iz] = 0.f;  m_values[Jz] = 0.f;  m_values[Kz] = 1.f;  m_values[Tz] = 0.f;
	m_values[Iw] = 0.f;  m_values[Jw] = 0.f;  m_values[Kw] = 0.f;  m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D)
{
	m_values[Ix] = iBasis2D.x;  m_values[Jx] = jBasis2D.x;	m_values[Kx] = 0.f;  m_values[Tx] = translation2D.x;
	m_values[Iy] = iBasis2D.y;	m_values[Jy] = jBasis2D.y;	m_values[Ky] = 0.f;  m_values[Ty] = translation2D.y;
	m_values[Iz] = 0.f;			m_values[Jz] = 0.f;			m_values[Kz] = 1.f;  m_values[Tz] = 0.f;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Kw] = 0.f;  m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D)
{
	m_values[Ix] = iBasis3D.x;  m_values[Jx] = jBasis3D.x;	m_values[Kx] = kBasis3D.x;  m_values[Tx] = translation3D.x;
	m_values[Iy] = iBasis3D.y;	m_values[Jy] = jBasis3D.y;	m_values[Ky] = kBasis3D.y;  m_values[Ty] = translation3D.y;
	m_values[Iz] = iBasis3D.z;	m_values[Jz] = jBasis3D.z;	m_values[Kz] = kBasis3D.z;  m_values[Tz] = translation3D.z;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Kw] = 0.f;			m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_values[Ix] = iBasis4D.x;  m_values[Jx] = jBasis4D.x;	m_values[Kx] = kBasis4D.x;  m_values[Tx] = translation4D.x;
	m_values[Iy] = iBasis4D.y;	m_values[Jy] = jBasis4D.y;	m_values[Ky] = kBasis4D.y;  m_values[Ty] = translation4D.y;
	m_values[Iz] = iBasis4D.z;	m_values[Jz] = jBasis4D.z;	m_values[Kz] = kBasis4D.z;  m_values[Tz] = translation4D.z;
	m_values[Iw] = iBasis4D.w;	m_values[Jw] = jBasis4D.w;	m_values[Kw] = kBasis4D.w;	m_values[Tw] = translation4D.w;
}


//----------------------------------------------------------------------------------------------------------
Mat44::Mat44(float const* sixteenValuesBasisMajor)
{
	for (int storageIndex = 0; storageIndex < 16; storageIndex++)
	{
		m_values[storageIndex] = sixteenValuesBasisMajor[storageIndex];
	}
}


//----------------------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateFromTransform( Transform const& transform )
{
	// 1. extract the rotation basis of the transform
	Vec3 iBasis3D = transform.m_rotation * Vec3(1.f, 0.f, 0.f);
	Vec3 jBasis3D = transform.m_rotation * Vec3(0.f, 1.f, 0.f);
	Vec3 kBasis3D = transform.m_rotation * Vec3(0.f, 0.f, 1.f);

	// 2. scale the basis vector
	iBasis3D *= transform.m_scale.x;
	jBasis3D *= transform.m_scale.y;
	kBasis3D *= transform.m_scale.z;

	// 3. extract the translation
	Vec3 translation3D = transform.m_position;

	// 4. create the matrix
	Mat44 transformMatrix = Mat44(iBasis3D, jBasis3D, kBasis3D, translation3D);
	return transformMatrix;
}


//----------------------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateTranslation2D(Vec2 const& translationXY)
{
	Mat44 translationMat;
	translationMat.m_values[Tx] = translationXY.x;
	translationMat.m_values[Ty] = translationXY.y;

	return translationMat;
}

Mat44 const Mat44::CreateTranslation3D(Vec3 const& translationXYZ)
{
	Mat44 translationMat;
	translationMat.m_values[Tx] = translationXYZ.x;
	translationMat.m_values[Ty] = translationXYZ.y;
	translationMat.m_values[Tz] = translationXYZ.z;

	return translationMat;
}

Mat44 const Mat44::CreateUniformScale2D(float uniformScaleXY)
{
	Mat44 uniformScale2DMat;
	uniformScale2DMat.m_values[Ix] *= uniformScaleXY;
	uniformScale2DMat.m_values[Jy] *= uniformScaleXY;

	return uniformScale2DMat;
}

Mat44 const Mat44::CreateUniformScale3D(float uniformScaleXYX)
{
	Mat44 uniformScale3DMat;
	uniformScale3DMat.m_values[Ix] *= uniformScaleXYX;
	uniformScale3DMat.m_values[Jy] *= uniformScaleXYX;
	uniformScale3DMat.m_values[Kz] *= uniformScaleXYX;

	return uniformScale3DMat;
}

Mat44 const Mat44::CreateNonUniformScale2D(Vec2 const& nonUniformScaleXY)
{
	Mat44 nonUniformScale2DMat;
	nonUniformScale2DMat.m_values[Ix] *= nonUniformScaleXY.x;
	nonUniformScale2DMat.m_values[Jy] *= nonUniformScaleXY.y;

	return nonUniformScale2DMat;
}

Mat44 const Mat44::CreateNonUniformScale3D(Vec3 const& nonUniformScaleXYZ)
{
	Mat44 nonUniformScale3DMat;
	nonUniformScale3DMat.m_values[Ix] *= nonUniformScaleXYZ.x;
	nonUniformScale3DMat.m_values[Jy] *= nonUniformScaleXYZ.y;
	nonUniformScale3DMat.m_values[Kz] *= nonUniformScaleXYZ.z;

	return nonUniformScale3DMat;
}

Mat44 const Mat44::CreateZRotationDegrees(float rotationDegreesAboutZ)
{
	float cosine = CosDegrees(rotationDegreesAboutZ);
	float sine = SinDegrees(rotationDegreesAboutZ);

	Mat44 zRotationDegreesMat;
	zRotationDegreesMat.m_values[Ix] = cosine;	zRotationDegreesMat.m_values[Jx] = -sine;
	zRotationDegreesMat.m_values[Iy] = sine;	zRotationDegreesMat.m_values[Jy] = cosine;

	return zRotationDegreesMat;
}

Mat44 const Mat44::CreateYRotationDegrees(float rotationDegreesAboutY)
{
	float cosine = CosDegrees(rotationDegreesAboutY);
	float sine = SinDegrees(rotationDegreesAboutY);

	Mat44 yRotationDegreesMat;
	yRotationDegreesMat.m_values[Ix] = cosine;	yRotationDegreesMat.m_values[Kx] = sine;
	yRotationDegreesMat.m_values[Iz] = -sine;	yRotationDegreesMat.m_values[Kz] = cosine;

	return yRotationDegreesMat;
}

Mat44 const Mat44::CreateXRotationDegrees(float rotationDegreesAboutX)
{
	float cosine = CosDegrees(rotationDegreesAboutX);
	float sine = SinDegrees(rotationDegreesAboutX);

	Mat44 xRotationDegreesMat;
	xRotationDegreesMat.m_values[Jy] = cosine;	xRotationDegreesMat.m_values[Ky] = -sine;
	xRotationDegreesMat.m_values[Jz] = sine;	xRotationDegreesMat.m_values[Kz] = cosine;

	return xRotationDegreesMat;
}

Vec2 const Mat44::TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const
{
	float x = m_values[Ix] * vectorQuantityXY.x		+	m_values[Jx] * vectorQuantityXY.y;
	float y = m_values[Iy] * vectorQuantityXY.x		+	m_values[Jy] * vectorQuantityXY.y;

	return Vec2(x, y);
}

Vec3 const Mat44::TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const
{
	float x = m_values[Ix] * vectorQuantityXYZ.x	+	m_values[Jx] * vectorQuantityXYZ.y	+ m_values[Kx] * vectorQuantityXYZ.z;
	float y = m_values[Iy] * vectorQuantityXYZ.x	+	m_values[Jy] * vectorQuantityXYZ.y	+ m_values[Ky] * vectorQuantityXYZ.z;
	float z = m_values[Iz] * vectorQuantityXYZ.x	+	m_values[Jz] * vectorQuantityXYZ.y	+ m_values[Kz] * vectorQuantityXYZ.z;
	
	return Vec3(x, y, z);
}

Vec2 const Mat44::TransformPosition2D(Vec2 const& positionXY) const
{
	float x = m_values[Ix] * positionXY.x	+	m_values[Jx] * positionXY.y		+ m_values[Tx];
	float y = m_values[Iy] * positionXY.x	+	m_values[Jy] * positionXY.y		+ m_values[Ty];

	return Vec2(x, y);
}

Vec3 const Mat44::TransformPosition3D(Vec3 const& positionXYZ) const
{
	float x = m_values[Ix] * positionXYZ.x	+	m_values[Jx] * positionXYZ.y	+	m_values[Kx] * positionXYZ.z	+ m_values[Tx];
	float y = m_values[Iy] * positionXYZ.x	+	m_values[Jy] * positionXYZ.y	+	m_values[Ky] * positionXYZ.z	+ m_values[Ty];
	float z = m_values[Iz] * positionXYZ.x	+	m_values[Jz] * positionXYZ.y	+	m_values[Kz] * positionXYZ.z	+ m_values[Tz];
	
	return Vec3(x, y, z);
}

Vec4 const Mat44::TransformHomogeneous3D(Vec4 const& homogeneousPoint3D) const
{
	float x = m_values[Ix] * homogeneousPoint3D.x	+	m_values[Jx] * homogeneousPoint3D.y		+	m_values[Kx] * homogeneousPoint3D.z		+	m_values[Tx] * homogeneousPoint3D.w;
	float y = m_values[Iy] * homogeneousPoint3D.x	+	m_values[Jy] * homogeneousPoint3D.y		+	m_values[Ky] * homogeneousPoint3D.z		+	m_values[Ty] * homogeneousPoint3D.w;
	float z = m_values[Iz] * homogeneousPoint3D.x	+	m_values[Jz] * homogeneousPoint3D.y		+	m_values[Kz] * homogeneousPoint3D.z		+	m_values[Tz] * homogeneousPoint3D.w;
	float w = m_values[Iw] * homogeneousPoint3D.x	+	m_values[Jw] * homogeneousPoint3D.y		+	m_values[Kw] * homogeneousPoint3D.z		+	m_values[Tw] * homogeneousPoint3D.w;
	
	return Vec4(x, y, z, w);
}

float* Mat44::GetAsFloatArray()
{
	return m_values;
}

float const* Mat44::GetAsFloatArray() const
{
	return m_values;
}

Vec2 const Mat44::GetIBasis2D() const
{
	Vec2 iBasis2D;
	iBasis2D.x = m_values[Ix];
	iBasis2D.y = m_values[Iy];

	return iBasis2D;
}

Vec2 const Mat44::GetJBasis2D() const
{
	Vec2 jBasis2D;
	jBasis2D.x = m_values[Jx];
	jBasis2D.y = m_values[Jy];

	return jBasis2D;
}

Vec2 const Mat44::GetTranslation2D() const
{
	Vec2 translation2D;
	translation2D.x = m_values[Tx];
	translation2D.y = m_values[Ty];

	return translation2D;
}

Vec3 const Mat44::GetIBasis3D() const
{
	Vec3 iBasis3D;
	iBasis3D.x = m_values[Ix];
	iBasis3D.y = m_values[Iy];
	iBasis3D.z = m_values[Iz];

	return iBasis3D;
}

Vec3 const Mat44::GetJBasis3D() const
{
	Vec3 jBasis3D;
	jBasis3D.x = m_values[Jx];
	jBasis3D.y = m_values[Jy];
	jBasis3D.z = m_values[Jz];

	return jBasis3D;
}

Vec3 const Mat44::GetKBasis3D() const
{
	Vec3 kBasis3D;
	kBasis3D.x = m_values[Kx];
	kBasis3D.y = m_values[Ky];
	kBasis3D.z = m_values[Kz];

	return kBasis3D;
}

Vec3 const Mat44::GetTranslation3D() const
{
	Vec3 translation3D;
	translation3D.x = m_values[Tx];
	translation3D.y = m_values[Ty];
	translation3D.z = m_values[Tz];

	return translation3D;
}

Vec4 const Mat44::GetIBasis4D() const
{
	Vec4 iBasis4D;
	iBasis4D.x = m_values[Ix];
	iBasis4D.y = m_values[Iy];
	iBasis4D.z = m_values[Iz];
	iBasis4D.w = m_values[Iw];

	return iBasis4D;
}

Vec4 const Mat44::GetJBasis4D() const
{
	Vec4 jBasis4D;
	jBasis4D.x = m_values[Jx];
	jBasis4D.y = m_values[Jy];
	jBasis4D.z = m_values[Jz];
	jBasis4D.w = m_values[Jw];

	return jBasis4D;
}

Vec4 const Mat44::GetKBasis4D() const
{
	Vec4 kBasis4D;
	kBasis4D.x = m_values[Kx];
	kBasis4D.y = m_values[Ky];
	kBasis4D.z = m_values[Kz];
	kBasis4D.w = m_values[Kw];

	return kBasis4D;
}

Vec4 const Mat44::GetTranslation4D() const
{
	Vec4 translation4D;
	translation4D.x = m_values[Tx];
	translation4D.y = m_values[Ty];
	translation4D.z = m_values[Tz];
	translation4D.w = m_values[Tw];

	return translation4D;
}

void Mat44::SetTranslation2D(Vec2 const& translationXY)
{
	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

void Mat44::SetTranslation3D(Vec3 const& translationXYZ)
{
	m_values[Tx] = translationXYZ.x;
	m_values[Ty] = translationXYZ.y;
	m_values[Tz] = translationXYZ.z;
	m_values[Tw] = 1.f;
}

void Mat44::SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D)
{
	m_values[Ix] = iBasis2D.x;  m_values[Jx] = jBasis2D.x;
	m_values[Iy] = iBasis2D.y;	m_values[Jy] = jBasis2D.y;
	m_values[Iz] = 0.f;			m_values[Jz] = 0.f;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;
}

void Mat44::SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY)
{
	m_values[Ix] = iBasis2D.x;  m_values[Jx] = jBasis2D.x;	m_values[Tx] = translationXY.x;
	m_values[Iy] = iBasis2D.y;	m_values[Jy] = jBasis2D.y;	m_values[Ty] = translationXY.y;
	m_values[Iz] = 0.f;			m_values[Jz] = 0.f;			m_values[Tz] = 0.f;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Tw] = 1.f;
}

void Mat44::SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D)
{
	m_values[Ix] = iBasis3D.x;  m_values[Jx] = jBasis3D.x;	m_values[Kx] = kBasis3D.x; 
	m_values[Iy] = iBasis3D.y;	m_values[Jy] = jBasis3D.y;	m_values[Ky] = kBasis3D.y; 
	m_values[Iz] = iBasis3D.z;	m_values[Jz] = jBasis3D.z;	m_values[Kz] = kBasis3D.z; 
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Kw] = 0.f;		
}

void Mat44::SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ)
{
	m_values[Ix] = iBasis3D.x;  m_values[Jx] = jBasis3D.x;	m_values[Kx] = kBasis3D.x;  m_values[Tx] = translationXYZ.x;
	m_values[Iy] = iBasis3D.y;	m_values[Jy] = jBasis3D.y;	m_values[Ky] = kBasis3D.y;  m_values[Ty] = translationXYZ.y;
	m_values[Iz] = iBasis3D.z;	m_values[Jz] = jBasis3D.z;	m_values[Kz] = kBasis3D.z;  m_values[Tz] = translationXYZ.z;
	m_values[Iw] = 0.f;			m_values[Jw] = 0.f;			m_values[Kw] = 0.f;			m_values[Tw] = 1.f;
}

void Mat44::SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_values[Ix] = iBasis4D.x;  m_values[Jx] = jBasis4D.x;	m_values[Kx] = kBasis4D.x;  m_values[Tx] = translation4D.x;
	m_values[Iy] = iBasis4D.y;	m_values[Jy] = jBasis4D.y;	m_values[Ky] = kBasis4D.y;  m_values[Ty] = translation4D.y;
	m_values[Iz] = iBasis4D.z;	m_values[Jz] = jBasis4D.z;	m_values[Kz] = kBasis4D.z;  m_values[Tz] = translation4D.z;
	m_values[Iw] = iBasis4D.w;	m_values[Jw] = jBasis4D.w;	m_values[Kw] = kBasis4D.w;	m_values[Tw] = translation4D.w;
}

void Mat44::Append(Mat44 const& appendThis)
{
	Mat44 copy = *this;

	float* my = copy.m_values;
	float const* append = appendThis.m_values;

	m_values[Ix] = my[Ix] * append[Ix]	+	my[Jx] * append[Iy]		+	my[Kx] * append[Iz]		+	my[Tx] * append[Iw];
	m_values[Iy] = my[Iy] * append[Ix]	+	my[Jy] * append[Iy]		+	my[Ky] * append[Iz]		+	my[Ty] * append[Iw];
	m_values[Iz] = my[Iz] * append[Ix]	+	my[Jz] * append[Iy]		+	my[Kz] * append[Iz]		+	my[Tz] * append[Iw];
	m_values[Iw] = my[Iw] * append[Ix]	+	my[Jw] * append[Iy]		+	my[Kw] * append[Iz]		+	my[Tw] * append[Iw];

	m_values[Jx] = my[Ix] * append[Jx]	+	my[Jx] * append[Jy]		+	my[Kx] * append[Jz]		+	my[Tx] * append[Jw];
	m_values[Jy] = my[Iy] * append[Jx]	+	my[Jy] * append[Jy]		+	my[Ky] * append[Jz]		+	my[Ty] * append[Jw];
	m_values[Jz] = my[Iz] * append[Jx]	+	my[Jz] * append[Jy]		+	my[Kz] * append[Jz]		+	my[Tz] * append[Jw];
	m_values[Jw] = my[Iw] * append[Jx]	+	my[Jw] * append[Jy]		+	my[Kw] * append[Jz]		+	my[Tw] * append[Jw];

	m_values[Kx] = my[Ix] * append[Kx]	+	my[Jx] * append[Ky]		+	my[Kx] * append[Kz]		+	my[Tx] * append[Kw];
	m_values[Ky] = my[Iy] * append[Kx]	+	my[Jy] * append[Ky]		+	my[Ky] * append[Kz]		+	my[Ty] * append[Kw];
	m_values[Kz] = my[Iz] * append[Kx]	+	my[Jz] * append[Ky]		+	my[Kz] * append[Kz]		+	my[Tz] * append[Kw];
	m_values[Kw] = my[Iw] * append[Kx]	+	my[Jw] * append[Ky]		+	my[Kw] * append[Kz]		+	my[Tw] * append[Kw];

	m_values[Tx] = my[Ix] * append[Tx]	+	my[Jx] * append[Ty]		+	my[Kx] * append[Tz]		+	my[Tx] * append[Tw];
	m_values[Ty] = my[Iy] * append[Tx]	+	my[Jy] * append[Ty]		+	my[Ky] * append[Tz]		+	my[Ty] * append[Tw];
	m_values[Tz] = my[Iz] * append[Tx]	+	my[Jz] * append[Ty]		+	my[Kz] * append[Tz]		+	my[Tz] * append[Tw];
	m_values[Tw] = my[Iw] * append[Tx]	+	my[Jw] * append[Ty]		+	my[Kw] * append[Tz]		+	my[Tw] * append[Tw];
}

void Mat44::AppendZRotation(float degreesRotationAboutZ)
{
	Mat44 zRotationDegreesMat = Mat44::CreateZRotationDegrees(degreesRotationAboutZ);
	Append(zRotationDegreesMat);
}

void Mat44::AppendYRotation(float degreesRotationAboutY)
{
	Mat44 yRotationDegreesMat = Mat44::CreateYRotationDegrees(degreesRotationAboutY);
	Append(yRotationDegreesMat);
}

void Mat44::AppendXRotation(float degreesRotationAboutX)
{
	Mat44 xRotationDegreesMat = Mat44::CreateXRotationDegrees(degreesRotationAboutX);
	Append(xRotationDegreesMat);
}

void Mat44::AppendTranslation2D(Vec2 const& translationXY)
{
	Mat44 translate2DMat = Mat44::CreateTranslation2D(translationXY);
	Append(translate2DMat);
}

void Mat44::AppendTranslation3D(Vec3 const& translationXYZ)
{
	Mat44 translate3DMat = Mat44::CreateTranslation3D(translationXYZ);
	Append(translate3DMat);
}

void Mat44::AppendScaleUniform2D(float uniformScaleXY)
{
	Mat44 scaleUniform2DMat = Mat44::CreateUniformScale2D(uniformScaleXY);
	Append(scaleUniform2DMat);
}

void Mat44::AppendScaleUniform3D(float uniformScaleXYZ)
{
	Mat44 scaleUniform3DMat = Mat44::CreateUniformScale3D(uniformScaleXYZ);
	Append(scaleUniform3DMat);
}

void Mat44::AppendScaleNonUniform2D(Vec2 const& nonUniformScaleXY)
{
	Mat44 scaleNonUniform2DMat = Mat44::CreateNonUniformScale2D(nonUniformScaleXY);
	Append(scaleNonUniform2DMat);
}

void Mat44::AppendScaleNonUniform3D(Vec3 const& nonUniformScaleXYZ)
{
	Mat44 scaleNonUniform3DMat = Mat44::CreateNonUniformScale3D(nonUniformScaleXYZ);
	Append(scaleNonUniform3DMat);
}


void Mat44::Transpose()
{
	Mat44 copy(m_values);

	m_values[Ix] = copy.m_values[Ix];
	m_values[Iy] = copy.m_values[Jx];
	m_values[Iz] = copy.m_values[Kx];
	m_values[Iw] = copy.m_values[Tx];
				   
	m_values[Jx] = copy.m_values[Iy];
	m_values[Jy] = copy.m_values[Jy];
	m_values[Jz] = copy.m_values[Ky];
	m_values[Jw] = copy.m_values[Ty];
				   
	m_values[Kx] = copy.m_values[Iz];
	m_values[Ky] = copy.m_values[Jz];
	m_values[Kz] = copy.m_values[Kz];
	m_values[Kw] = copy.m_values[Tz];
				   
	m_values[Tx] = copy.m_values[Iw];
	m_values[Ty] = copy.m_values[Jw];
	m_values[Tz] = copy.m_values[Kw];
	m_values[Tw] = copy.m_values[Tw];
}

// This function computes the orthonormal inverse of a 4x4 matrix.
// The matrix is assumed to represent a rigid body transformation,
// i.e., it contains only rotation and translation.
Mat44 Mat44::GetOrthonormalInverse()
{
	// Create a new matrix to store the orthonormal inverse
	Mat44 orthoNormalInverse(m_values);

	// Transpose the rotation part of the matrix
	orthoNormalInverse.m_values[ Ix ] = m_values[ Ix ];
	orthoNormalInverse.m_values[ Iy ] = m_values[ Jx ];
	orthoNormalInverse.m_values[ Iz ] = m_values[ Kx ];

	orthoNormalInverse.m_values[ Jx ] = m_values[ Iy ];
	orthoNormalInverse.m_values[ Jy ] = m_values[ Jy ];
	orthoNormalInverse.m_values[ Jz ] = m_values[ Ky ];

	orthoNormalInverse.m_values[ Kx ] = m_values[ Iz ];
	orthoNormalInverse.m_values[ Ky ] = m_values[ Jz ];
	orthoNormalInverse.m_values[ Kz ] = m_values[ Kz ];

	// Compute the new translation part
	float negTx = -m_values[ Tx ];
	float negTy = -m_values[ Ty ];
	float negTz = -m_values[ Tz ];

	orthoNormalInverse.m_values[ Tx ] = m_values[ Ix ] * negTx + m_values[ Iy ] * negTy + m_values[ Iz ] * negTz;
	orthoNormalInverse.m_values[ Ty ] = m_values[ Jx ] * negTx + m_values[ Jy ] * negTy + m_values[ Jz ] * negTz;
	orthoNormalInverse.m_values[ Tz ] = m_values[ Kx ] * negTx + m_values[ Ky ] * negTy + m_values[ Kz ] * negTz;

	// Return the computed orthonormal inverse matrix
	return orthoNormalInverse;
}

void Mat44::Orthonormalize_XFwd_YLeft_ZUp()
{
	// fix i basis vector
	Vec3 iBasis3D = GetIBasis3D();
	iBasis3D.Normalize();
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;

	// fix k basis vector
	Vec3 kBasis3D = GetJBasis3D();
	
		// 1. get k projected onto i
		float k_ProjectedLengthOn_i = DotProduct3D(kBasis3D, iBasis3D);
		Vec3 k_ProjectedOnto_i = iBasis3D * k_ProjectedLengthOn_i;

		// 2. get parts of k basis vector, not in i basis vector
		Vec3 k_not_i = kBasis3D - k_ProjectedOnto_i;
		k_not_i.Normalize();
		kBasis3D = k_not_i;

	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;


	// fix j basis vector 
	Vec3 jBasis3D = CrossProduct3D(iBasis3D, kBasis3D);
	jBasis3D.Normalize();
	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
}

Mat44 Mat44::CreateOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Mat44 projection;
	
	projection.m_values[Ix] = 2.f / (right - left); 
	projection.m_values[Jy] = 2.f / (top - bottom);
	projection.m_values[Kz] = 1.f / (zFar - zNear);

	projection.m_values[Tx] = (left + right) / (left - right);
	projection.m_values[Ty] = (bottom + top) / (bottom - top);
	projection.m_values[Tz] = zNear / (zNear - zFar);

	return projection;
}

// I do not understand this, at all
Mat44 Mat44::CreatePerspectiveProjection(float fovYDegrees, float aspect, float zNear, float zFar)
{
	float scaleY;
	float scaleX;
	float scaleZ;
	float translateZ;

	Mat44 projection;

	projection.m_values[Jy] = scaleY = CosDegrees(fovYDegrees * 0.5f) / SinDegrees(fovYDegrees * 0.5f);  // equals 1 if vertical Field of View is 90
	projection.m_values[Ix] = scaleX = scaleY / aspect; // equals scaleY if screen is square (aspect=1.0); equals 1 if square screen and FOV 90
	projection.m_values[Kz] = scaleZ = zFar / (zFar - zNear);
	projection.m_values[Kw] = 1.0f; // this puts Z into the W component (in preparation for the hardware w-divide)
	
	projection.m_values[Tz] = translateZ = (zNear * zFar) / (zNear - zFar);
	projection.m_values[Tw] = 0.0f;  // Otherwise we would be putting Z+1 (instead of Z) into the W component

	return projection;
}
