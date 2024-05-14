#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

//#define PERFORMANCE_MODE


// static consts
EulerAngles const EulerAngles::ZERO = EulerAngles(0.f, 0.f, 0.f);


EulerAngles::EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees) :
	m_yawDegrees(yawDegrees), m_pitchDegrees(pitchDegrees), m_rollDegrees(rollDegrees)
{
}

Vec3 EulerAngles::GetVector_XFwd() const
{
	Vec3 xFwd;
	Vec3 yLeft;
	Vec3 zUp;

	GetAsVectors_XFwd_YLeft_ZUp(xFwd, yLeft, zUp);

	return xFwd;
}

void EulerAngles::GetAsVectors_XFwd_YLeft_ZUp(Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const
{
#ifndef PERFORMANCE_MODE

	Mat44 matrix = GetAsMatrix_XFwd_YLeft_ZUp();

	out_forwardIBasis = matrix.GetIBasis3D();
	out_leftJBasis = matrix.GetJBasis3D();
	out_upKBasis = matrix.GetKBasis3D();

#endif
#ifdef PERFORMANCE_MODE

	// TODO: add performance mode
	UNUSED(out_forwardIBasis);
	UNUSED(out_leftJBasis);
	UNUSED(out_upKBasis);

#endif
	
}

Mat44 EulerAngles::GetAsMatrix_XFwd_YLeft_ZUp() const
{
	Mat44 matrix;
	matrix.AppendZRotation(m_yawDegrees);
	matrix.AppendYRotation(m_pitchDegrees);
	matrix.AppendXRotation(m_rollDegrees);

	return matrix;
}

void EulerAngles::SetFromText(char const* text)
{
	Strings splitStrings = SplitStringOnDelimiter(text, ',');
	std::string yawDegreesStr = splitStrings[ 0 ];
	std::string pitchDegreesStr = splitStrings[ 1 ];
	std::string rollDegreesStr = splitStrings[ 2 ];

	m_yawDegrees = ( float ) atof(yawDegreesStr.c_str());
	m_pitchDegrees = ( float ) atof(pitchDegreesStr.c_str());
	m_rollDegrees = ( float ) atof(rollDegreesStr.c_str());
}


//----------------------------------------------------------------------------------------------------------

#define MARGIN_OF_ERROR 0.0001f
bool EulerAngles::ComponentWiseEquals( EulerAngles const& compare ) const
{
	bool yawEquals	 = ( fabsf( m_yawDegrees - compare.m_yawDegrees ) <= MARGIN_OF_ERROR );
	bool pitchEquals = ( fabsf( m_pitchDegrees - compare.m_pitchDegrees ) <= MARGIN_OF_ERROR );
	bool rollEquals	 = ( fabsf( m_rollDegrees - compare.m_rollDegrees ) <= MARGIN_OF_ERROR );

	bool allEquals = ( yawEquals && pitchEquals && rollEquals );

	return allEquals;
}

