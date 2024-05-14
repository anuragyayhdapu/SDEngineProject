#include "Engine/Math/Spline.hpp"

Spline::Spline(std::vector<Vec2> const& positions)
{
	ComputeSplinePoints(positions);

	ComputeSplineSegments();
}

void Spline::ComputeSplinePoints(std::vector<Vec2> const& positions)
{
	if ( positions.size() < 2 )
	{
		return;
	}

	Point startPoint;
	startPoint.m_position = positions[ 0 ];
	startPoint.m_velocity = Vec2::ZERO;

	m_splinePoints.push_back(startPoint);

	Point endPoint;
	endPoint.m_position = positions[ positions.size() - 1 ];
	endPoint.m_velocity = Vec2::ZERO;

	for ( int index = 1; index < positions.size() - 1; index++ )
	{
		Vec2 const& position = positions[ index ];
		Point pointOnSpline;
		pointOnSpline.m_position = position;

		// compute hermite velocity
		Vec2 const& prevPosition = positions[ index - 1 ];
		Vec2 const& nextPosition = positions[ index + 1 ];
		pointOnSpline.m_velocity = ( nextPosition - prevPosition ) * 0.5f;

		m_splinePoints.push_back(pointOnSpline);
	}

	m_splinePoints.push_back(endPoint);
}

void Spline::ComputeSplineSegments()
{
	for ( int index = 0; index < m_splinePoints.size() - 1; index += 1 )
	{
		Point cureStartPoint = m_splinePoints[ index ];
		Point curveEndPoint = m_splinePoints[ index + 1 ];
		CubicBezierCurve2D hermiteCurve = CubicBezierCurve2D::ComposeFromHermite2D(cureStartPoint.m_position, cureStartPoint.m_velocity, curveEndPoint.m_position, curveEndPoint.m_velocity);

		m_splineSegments.push_back(hermiteCurve);
	}
}

Vec2 Spline::EvaluateAtParametric(float parametricZeroToOne) const
{
	int totalSegments = ( int ) m_splineSegments.size();

	float splineParametric = totalSegments * parametricZeroToOne;
	int curveIndex = ( int ) splineParametric;
	float curveLocalParametric = splineParametric - curveIndex;

	CubicBezierCurve2D const& curve = m_splineSegments[ curveIndex ];
	Vec2 pointOnCurve = curve.EvaluateAtParametric(curveLocalParametric);
	return pointOnCurve;
}


float Spline::GetApproximateLength(int numOfSubdivision) const
{
	float approximateLength = 0.f;

	for ( int index = 0; index < m_splineSegments.size(); index++ )
	{
		CubicBezierCurve2D const& splineSegment = m_splineSegments[ index ];
		float length = splineSegment.GetApproximateLength(numOfSubdivision);

		approximateLength += length;
	}

	return approximateLength;
}


Vec2 Spline::EvaluateAtApproximateDistance(float distanceAlongCurve, int numOfSubdivisions) const
{
	Vec2 pointAtApproximateDistance = m_splinePoints[ 0 ].m_position;
	float distanceTravelled = 0.f;

	for ( int index = 0; index < m_splineSegments.size(); index++ )
	{
		CubicBezierCurve2D const& splineSegment = m_splineSegments[ index ];
		float segmentLength = splineSegment.GetApproximateLength(numOfSubdivisions);

		if ( distanceTravelled + segmentLength > distanceAlongCurve )
		{
			float distanceAlongCurrentSegment = distanceAlongCurve - distanceTravelled;
			pointAtApproximateDistance = splineSegment.EvaluateAtApproximateDistance(distanceAlongCurrentSegment, numOfSubdivisions);

			break;
		}
		else
		{
			distanceTravelled += segmentLength;
		}
	}

	return pointAtApproximateDistance;
}