#include "Engine/Animation/AnimChannel.hpp"


//----------------------------------------------------------------------------------------------------------
Transform AnimChannel::Sample( Transform const& reference, float sampleTimeMilliSeconds, bool isLooping, bool dontSampleTranslation ) const
{
	Transform result = reference;

	// position
	if (dontSampleTranslation)
	{
		// still sample z
		if ( !m_positionCurve.IsEmpty() )
		{
			result.m_position	= m_positionCurve.Sample( sampleTimeMilliSeconds, isLooping );
			result.m_position.x = 0.f;
			result.m_position.y = 0.f;
		}
	}
	else // sample x, y and z
	{
		if ( !m_positionCurve.IsEmpty() )
		{
			result.m_position = m_positionCurve.Sample( sampleTimeMilliSeconds, isLooping );
		}
	}
	
	// rotation
	if ( !m_rotationCurve.IsEmpty() )
	{
		result.m_rotation = m_rotationCurve.Sample( sampleTimeMilliSeconds, isLooping );
	}

	// scale
	if ( !m_scaleCurve.IsEmpty() )
	{
		result.m_scale = m_scaleCurve.Sample( sampleTimeMilliSeconds, isLooping );
	}

	return result;
}


//----------------------------------------------------------------------------------------------------------
bool AnimChannel::IsValid() const
{
	bool atleastOneCurveNotEmpty = !m_positionCurve.m_keyframes.empty() ||
								   !m_rotationCurve.m_keyframes.empty() ||
								   !m_scaleCurve.m_keyframes.empty();
	return atleastOneCurveNotEmpty;
}


//----------------------------------------------------------------------------------------------------------
float AnimChannel::GetEndTimeMilliSeconds() const
{
	float endTimeMilliSeconds = 0.f;

	float positionEndTime = m_positionCurve.GetEndTimeMilliSeconds();
	if (positionEndTime > endTimeMilliSeconds)
	{
		endTimeMilliSeconds = positionEndTime;
	}

	float rotationEndTime = m_rotationCurve.GetEndTimeMilliSeconds();
	if (rotationEndTime > endTimeMilliSeconds)
	{
		endTimeMilliSeconds = rotationEndTime;
	}

	float scalarEndTime = m_scaleCurve.GetEndTimeMilliSeconds();
	if (scalarEndTime > endTimeMilliSeconds)
	{
		endTimeMilliSeconds = scalarEndTime;
	}

	return endTimeMilliSeconds;
}
