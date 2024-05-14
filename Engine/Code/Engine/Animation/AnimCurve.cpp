#include "Engine/Animation/AnimCurve.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



//----------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------
float FloatAnimCurve::Sample( float sampleTimeMilliSeconds, bool isLooping ) const
{
	if ( m_keyframes.size() == 1 )
	{
		float firstFramevalue = m_keyframes[ 0 ].m_value;
		return firstFramevalue;
	}

	// 1. adjust time to fit inside the curve
	float			 curveStartTime = m_keyframes[ 0 ].m_timeMilliSeconds;
	int				 lastIndex		= ( int ) m_keyframes.size() - 1;
	float			 curveEndTime	= m_keyframes[ lastIndex ].m_timeMilliSeconds;
	AnimPlaybackType playbackType	= isLooping ? AnimPlaybackType::LOOP : AnimPlaybackType::ONCE;
	sampleTimeMilliSeconds			= AdjustSampleTimeToFitInsideRange( sampleTimeMilliSeconds, curveStartTime, curveEndTime, playbackType );

	// 2. get the pair of keyframes that surround given time
	int currentKeyframeIndex = GetKeyframeIndexAtSampleTime( m_keyframes, sampleTimeMilliSeconds );
	int nextKeyframeIndex	 = currentKeyframeIndex + 1;

	if ( m_interpolationType == AnimInterpolationType::Linear )
	{
		// 3. lerp to the time and get the keyframes
		float currentKeyframeTime  = m_keyframes[ currentKeyframeIndex ].m_timeMilliSeconds;
		float currentKeyframeValue = m_keyframes[ currentKeyframeIndex ].m_value;
		float nextKeyframeTime	   = m_keyframes[ nextKeyframeIndex ].m_timeMilliSeconds;
		float nextKeyframeValue	   = m_keyframes[ nextKeyframeIndex ].m_value;

		float sampleResult = RangeMap( sampleTimeMilliSeconds, currentKeyframeTime, nextKeyframeTime, currentKeyframeValue, nextKeyframeValue );
		return sampleResult;
	}

	// TODO: think of an appropriate return value
	return 0.f;
}


//----------------------------------------------------------------------------------------------------------
float FloatAnimCurve::GetEndTimeMilliSeconds() const
{
	if ( m_keyframes.empty() )
	{
		return 0.f;
	}
	else
	{
		int	  lastIndex			  = (int) m_keyframes.size() - 1;
		float endTimeMilliSeconds = m_keyframes[ lastIndex ].m_timeMilliSeconds;

		return endTimeMilliSeconds;
	}
}


//----------------------------------------------------------------------------------------------------------
Vec3 Vec3AnimCurve::Sample( float sampleTimeMilliSeconds, bool isLooping ) const
{
	if ( m_keyframes.size() == 1 )
	{
		Vec3 firstFramevalue = m_keyframes[ 0 ].m_value;
		return firstFramevalue;
	}

	// 1. adjust time to fit inside the curve
	float			 curveStartTime = m_keyframes[ 0 ].m_timeMilliSeconds;
	int				 lastIndex		= ( int ) m_keyframes.size() - 1;
	float			 curveEndTime	= m_keyframes[ lastIndex ].m_timeMilliSeconds;
	AnimPlaybackType playbackType	= isLooping ? AnimPlaybackType::LOOP : AnimPlaybackType::ONCE;
	sampleTimeMilliSeconds			= AdjustSampleTimeToFitInsideRange( sampleTimeMilliSeconds, curveStartTime, curveEndTime, playbackType );

	// 2. get the pair of keyframes that surround given time
	int currentKeyframeIndex = GetKeyframeIndexAtSampleTime( m_keyframes, sampleTimeMilliSeconds );
	int nextKeyframeIndex	 = currentKeyframeIndex + 1;

	if ( m_interpolationType == AnimInterpolationType::Linear )
	{
		// 3. lerp to the time and get the keyframes
		float		currentKeyframeTime	 = m_keyframes[ currentKeyframeIndex ].m_timeMilliSeconds;
		Vec3 const& currentKeyframeValue = m_keyframes[ currentKeyframeIndex ].m_value;
		float		nextKeyframeTime	 = m_keyframes[ nextKeyframeIndex ].m_timeMilliSeconds;
		Vec3 const& nextKeyframeValue	 = m_keyframes[ nextKeyframeIndex ].m_value;

		float xSampleResult = RangeMap( sampleTimeMilliSeconds, currentKeyframeTime, nextKeyframeTime, currentKeyframeValue.x, nextKeyframeValue.x );
		float ySampleResult = RangeMap( sampleTimeMilliSeconds, currentKeyframeTime, nextKeyframeTime, currentKeyframeValue.y, nextKeyframeValue.y );
		float zSampleResult = RangeMap( sampleTimeMilliSeconds, currentKeyframeTime, nextKeyframeTime, currentKeyframeValue.z, nextKeyframeValue.z );

		Vec3 sampleResult( xSampleResult, ySampleResult, zSampleResult );
		return sampleResult;
	}
	else // TODO: later
	{
		return Vec3::ZERO;
	}
}


//----------------------------------------------------------------------------------------------------------
float Vec3AnimCurve::GetEndTimeMilliSeconds() const
{
	if ( m_keyframes.empty() )
	{
		return 0.f;
	}
	else
	{
		int	  lastIndex			  = ( int ) m_keyframes.size() - 1;
		float endTimeMilliSeconds = m_keyframes[ lastIndex ].m_timeMilliSeconds;

		return endTimeMilliSeconds;
	}
}


//----------------------------------------------------------------------------------------------------------
void Vec3AnimCurve::GetMaxAndMinZValuesFromCurve( float& zMin, float& zMax )
{
	zMin = FLT_MAX;
	zMax = FLT_MIN;
	for ( int curveIndex = 0; curveIndex < m_keyframes.size(); curveIndex++ )
	{
		Vector3Keyframe const& keyFrameValue = m_keyframes[ curveIndex ];
		float				   zValue		 = keyFrameValue.m_value.z;

		if ( zValue < zMin )
		{
			zMin = zValue;
		}

		if ( zValue > zMax )
		{
			zMax = zValue;
		}
	}
}


//----------------------------------------------------------------------------------------------------------
void Vec3AnimCurve::GetMaxAndMinXValuesFromCurve( float& xMin, float& xMax )
{
	xMin = FLT_MAX;
	xMax = FLT_MIN;
	for ( int curveIndex = 0; curveIndex < m_keyframes.size(); curveIndex++ )
	{
		Vector3Keyframe const& keyFrameValue = m_keyframes[ curveIndex ];
		float				   xValue		 = keyFrameValue.m_value.x;

		if ( xValue < xMin )
		{
			xMin = xValue;
		}

		if ( xValue > xMax )
		{
			xMax = xValue;
		}
	}
}


//----------------------------------------------------------------------------------------------------------
Vector3Keyframe const& Vec3AnimCurve::GetKeyframeAtIndex( unsigned int index ) const
{
	Vector3Keyframe const& keyFrame = m_keyframes[ index ];
	return keyFrame;
}


//----------------------------------------------------------------------------------------------------------
Vector3Keyframe const& Vec3AnimCurve::GetKeyframeAtFirstIndex() const
{
	Vector3Keyframe const& firstKeyframe = m_keyframes[ 0 ];
	return firstKeyframe;
}


//----------------------------------------------------------------------------------------------------------
Vector3Keyframe const& Vec3AnimCurve::GetKeyframeAtLastIndex() const
{
	size_t				   lastIndex	= m_keyframes.size() - 1;
	Vector3Keyframe const& lastKeyframe = m_keyframes[ lastIndex ];
	return lastKeyframe;
}


//----------------------------------------------------------------------------------------------------------
Quaternion QuaternionAnimCurve::Sample( float sampleTimeMilliSeconds, bool isLooping ) const
{
	if ( m_keyframes.size() == 1 )
	{
		Quaternion firstFramevalue = m_keyframes[ 0 ].m_value;
		return firstFramevalue;
	}

	// 1. adjust time to fit inside the curve
	float			 curveStartTime = m_keyframes[ 0 ].m_timeMilliSeconds;
	int				 lastIndex		= ( int ) m_keyframes.size() - 1;
	float			 curveEndTime	= m_keyframes[ lastIndex ].m_timeMilliSeconds;
	AnimPlaybackType playbackType	= isLooping ? AnimPlaybackType::LOOP : AnimPlaybackType::ONCE;
	sampleTimeMilliSeconds			= AdjustSampleTimeToFitInsideRange( sampleTimeMilliSeconds, curveStartTime, curveEndTime, playbackType );

	// 2. get the pair of keyframes that surround given time
	int currentKeyframeIndex = GetKeyframeIndexAtSampleTime( m_keyframes, sampleTimeMilliSeconds );
	int nextKeyframeIndex	 = currentKeyframeIndex + 1;

	if ( m_interpolationType == AnimInterpolationType::Linear )
	{
		// 3. lerp to the time and get the keyframes
		float			  currentKeyframeTime  = m_keyframes[ currentKeyframeIndex ].m_timeMilliSeconds;
		Quaternion const& currentKeyframeValue = m_keyframes[ currentKeyframeIndex ].m_value;
		float			  nextKeyframeTime	   = m_keyframes[ nextKeyframeIndex ].m_timeMilliSeconds;
		Quaternion const& nextKeyframeValue	   = m_keyframes[ nextKeyframeIndex ].m_value;

		Quaternion sampleResult = NormalizedLerp( sampleTimeMilliSeconds, currentKeyframeTime, nextKeyframeTime, currentKeyframeValue, nextKeyframeValue );
		return sampleResult;
	}

	// TODO: remove later
	return Quaternion::IDENTITY;
}


//----------------------------------------------------------------------------------------------------------
float QuaternionAnimCurve::GetEndTimeMilliSeconds() const
{
	if ( m_keyframes.empty() )
	{
		return 0.f;
	}
	else
	{
		int	  lastIndex			  = ( int ) m_keyframes.size() - 1;
		float endTimeMilliSeconds = m_keyframes[ lastIndex ].m_timeMilliSeconds;

		return endTimeMilliSeconds;
	}
}



//----------------------------------------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------------------------------------




