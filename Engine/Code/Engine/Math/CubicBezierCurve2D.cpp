#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"



CubicBezierCurve2D::CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos) :
	m_startPos(startPos), m_guidePos1(guidePos1), m_guidePos2(guidePos2), m_endPos(endPos)
{
	// set the Hermite curve velocities
	m_startVelocity = 3.f * ( m_guidePos1 - startPos );
	m_endVelocity = 3.f * ( m_endPos - m_guidePos2 );
}

CubicBezierCurve2D CubicBezierCurve2D::ComposeFromHermite2D(Vec2 startPos, Vec2 startVelocity, Vec2 endPos, Vec2 endVelocity)
{
	CubicBezierCurve2D cubicBezierCurve2D = CubicBezierCurve2D();
	cubicBezierCurve2D.m_startPos = startPos;
	cubicBezierCurve2D.m_startVelocity = startVelocity;
	cubicBezierCurve2D.m_endPos = endPos;
	cubicBezierCurve2D.m_endVelocity = endVelocity;

	// calculate the guide pos 1 and guide pos 2
	cubicBezierCurve2D.m_guidePos1 = ( ( startVelocity + ( 3.f * startPos ) ) / 3.f );
	cubicBezierCurve2D.m_guidePos2 = ( ( ( -1.f * endVelocity ) + ( 3.f * endPos ) ) / 3.f );

	return cubicBezierCurve2D;
}


Vec2 CubicBezierCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	float x = ComputeCubicBezier1D(m_startPos.x, m_guidePos1.x, m_guidePos2.x, m_endPos.x, parametricZeroToOne);
	float y = ComputeCubicBezier1D(m_startPos.y, m_guidePos1.y, m_guidePos2.y, m_endPos.y, parametricZeroToOne);

	Vec2 pointOnCurve(x, y);
	return pointOnCurve;
}

float CubicBezierCurve2D::GetApproximateLength(int numOfSubdivision) const
{
	float approximateLength = 0.f;

	float deltaT = 1.f / numOfSubdivision;
	for ( float t = 0.f; t < 1.f; t += deltaT )
	{
		Vec2 point = EvaluateAtParametric(t);
		float nextT = t + deltaT;
		Vec2  nextPoint = EvaluateAtParametric(nextT);

		Vec2 displacement = nextPoint - point;
		float length = displacement.GetLength();

		approximateLength += length;
	}

	return approximateLength;
}

Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numOfSubdivions) const
{
	Vec2 pointAtApproximateDistance = m_startPos;
	float distanceTravelled = 0.f;

	float deltaT = 1.f / numOfSubdivions;
	for ( float t = 0.f; t < 1.f; t += deltaT )
	{
		Vec2 point = EvaluateAtParametric(t);
		float nextT = t + deltaT;
		Vec2  nextPoint = EvaluateAtParametric(nextT);

		Vec2 displacement = nextPoint - point;
		float length = displacement.GetLength();
		distanceTravelled += length;

		if ( distanceTravelled > distanceAlongCurve )
		{
			float remainingLength = distanceTravelled - distanceAlongCurve;
			Vec2 direction = displacement.GetNormalized();

			pointAtApproximateDistance = nextPoint - ( direction * remainingLength );
			break;
		}
	}

	return pointAtApproximateDistance;
}

void CubicBezierCurve2D::SetStartVelocity(Vec2 const& startVelocity)
{
	m_startVelocity = startVelocity;

	m_guidePos1 = ( ( m_startVelocity + ( 3.f * m_startPos ) ) / 3.f );
}

void CubicBezierCurve2D::SetEndVelocity(Vec2 const& endVelocity)
{
	m_endVelocity = endVelocity;

	m_guidePos2 = ( ( ( -1.f * m_endVelocity ) + ( 3.f * m_endPos ) ) / 3.f );
}
