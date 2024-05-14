#include "Engine/Animation/AnimCrossFadeController.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//----------------------------------------------------------------------------------------------------------
AnimCrossfadeController::AnimCrossfadeController()
{
}


//----------------------------------------------------------------------------------------------------------
void AnimCrossfadeController::Update( float deltaSeconds )
{
	float deltaMilliseconds = deltaSeconds * 1000.f;
	m_currentPlaybackTimeMilliseconds += deltaMilliseconds;

	// sample the current clip
	m_currentAnimation->Sample( m_currentPlaybackTimeMilliseconds, m_currentSampledPose );

	for ( int index = 0; index < m_targets.size(); index++ )
	{
		AnimCrossFadeTarget& target = m_targets[ index ];

		target.m_targetAnimation->Sample( target.m_playbackTimeMilliseconds, target.m_sampledPose );

		// blend value is a range map between elapsed time and fade duration
		float blendValue = RangeMapClamped( target.m_elapsedTimeInFadeMilliseconds, 0.f, target.m_fadeDurationMilliseconds, 0.f, 1.f );

		//DebuggerPrintf( "blend value: %.2f, elapsedTime: %.2f\n", blendValue, target.m_elapsedTimeInFadeMilliseconds );

		AnimPose::Blend( m_currentSampledPose, m_currentSampledPose, target.m_sampledPose, blendValue, -1 );


		target.m_playbackTimeMilliseconds += deltaMilliseconds;
		target.m_elapsedTimeInFadeMilliseconds += deltaMilliseconds;
	}

	for ( int index = 0; index < m_targets.size(); index++ )
	{
		AnimCrossFadeTarget& target = m_targets[ index ];

		if ( target.m_elapsedTimeInFadeMilliseconds > target.m_fadeDurationMilliseconds )
		{
			m_currentAnimation				  = target.m_targetAnimation;
			m_currentPlaybackTimeMilliseconds = target.m_playbackTimeMilliseconds;
			m_targets.erase( m_targets.begin() + index );

			DebuggerPrintf( "Animation Cross faded: %s\n", m_currentAnimation->m_name.c_str() );

			break;
		}
	}
}