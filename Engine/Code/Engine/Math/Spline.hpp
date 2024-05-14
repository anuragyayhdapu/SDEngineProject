#pragma once

#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/Vec2.hpp"

#include <vector>


class Spline
{
public:
	Spline() { }
	Spline(std::vector<Vec2> const& positions);

	struct Point
	{
		Vec2 m_position = Vec2::ZERO;	// hermite curve positions
		Vec2 m_velocity = Vec2::ZERO;	// hermite velocities according to Catmull-Rom algorithm
	};

	const std::vector<Point>& GetSplinePoints() const { return m_splinePoints; }
	const std::vector<CubicBezierCurve2D>& GetSplineSegments() const { return m_splineSegments; }

	Vec2 EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int numOfSubdivision = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numOfSubdivisions = 64) const;

private:
	std::vector<Point> m_splinePoints;
	void ComputeSplinePoints(std::vector<Vec2> const& positions);

	std::vector<CubicBezierCurve2D>  m_splineSegments;
	void ComputeSplineSegments();
};