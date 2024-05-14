#pragma once

#include "Engine/Math/Vec2.hpp"

struct CubicHermiteCurve2D
{

};

class CubicBezierCurve2D
{
public:
	CubicBezierCurve2D() { }
	CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos);

	static CubicBezierCurve2D ComposeFromHermite2D(Vec2 startPos, Vec2 startVelocity, Vec2 endPos, Vec2 endVelocity);

	Vec2 EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int numOfSubdivision = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numOfSubdivions = 64) const;


	Vec2 GetStartPos() const { return m_startPos; }
	Vec2 GetGuidePos1() const { return m_guidePos1; }
	Vec2 GetGuidePos2() const { return m_guidePos2; }
	Vec2 GetEndPos() const { return m_endPos; }

	Vec2 GetStartVelocity() const { return m_startVelocity; }
	Vec2 GetEndVelocity() const { return m_endVelocity; }
	void SetStartVelocity(Vec2 const& startVelocity);
	void SetEndVelocity(Vec2 const& endVelocity);

private:
	Vec2 m_startPos = Vec2::ZERO;
	Vec2 m_guidePos1 = Vec2::ZERO;
	Vec2 m_guidePos2 = Vec2::ZERO;
	Vec2 m_endPos = Vec2::ZERO;

	Vec2 m_startVelocity = Vec2::ZERO;
	Vec2 m_endVelocity = Vec2::ZERO;
};