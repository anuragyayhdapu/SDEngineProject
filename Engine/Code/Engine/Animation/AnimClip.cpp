#include "Engine/Animation/AnimClip.hpp"
#include "Engine/Animation/AnimPose.hpp"
#include "Engine/Animation/AnimUtils.hpp"
#include "Engine/Animation/FbxFileImporter.hpp"


//----------------------------------------------------------------------------------------------------------
std::map<std::string, AnimClip*> AnimClip::s_animClipRegistery;


//----------------------------------------------------------------------------------------------------------
float AnimClip::Sample( float sampleTimeMilliSeconds, AnimPose& outPose ) const
{
	AnimPlaybackType playbackType		= m_isLooping ? AnimPlaybackType::LOOP : AnimPlaybackType::ONCE;
	float			 adjustedSampleTime = AdjustSampleTimeToFitInsideRange( sampleTimeMilliSeconds, m_startTimeMilliseconds, m_endTimeMilliseconds, playbackType );

	// 1. for every Animation channel is this clip
	for ( int index = 0; index < m_animChannels.size(); index++ )
	{
		AnimChannel const& animChannel = m_animChannels[ index ];
		int				   jointId	   = animChannel.m_jointId;

		// 2. sample the Animation Curve for that joint id
		Transform defaultLocalTransform = outPose.GetLocalTransformOfJoint( jointId );

		bool	  removeRootMotionTranslation = ( index == 0 ) && m_removeRootMotion;
		Transform animatedLocalTransform	  = animChannel.Sample( defaultLocalTransform, adjustedSampleTime, m_isLooping, removeRootMotionTranslation );

		// 3. set the local transform
		outPose.SetLocalTransformOfJoint( animatedLocalTransform, jointId );
	}

	return adjustedSampleTime;
}


//----------------------------------------------------------------------------------------------------------
AnimChannel& AnimClip::CreateOrGetAnimChannel( int jointIdToGet )
{
	// 1. try to get the Anim Channel from joint id
	for ( int index = 0; index < m_animChannels.size(); index++ )
	{
		AnimChannel& animChannel = m_animChannels[ index ];
		int			 jointId	 = animChannel.m_jointId;
		if ( jointIdToGet == jointId )
		{
			return animChannel;
		}
	}

	// 2. if failed create the new Anim Channel with new joint id
	AnimChannel newAnimChannel;
	newAnimChannel.m_jointId = jointIdToGet;
	m_animChannels.push_back( newAnimChannel );

	// get the last anim channel
	int			 lastIndex			   = ( int ) m_animChannels.size() - 1;
	AnimChannel& justPushedAnimChannel = m_animChannels[ lastIndex ];
	return justPushedAnimChannel;
}


//----------------------------------------------------------------------------------------------------------
float AnimClip::GetStartTime() const
{
	return m_startTimeMilliseconds;
}


//----------------------------------------------------------------------------------------------------------
float AnimClip::GetEndTime() const
{
	return m_endTimeMilliseconds;
}


//----------------------------------------------------------------------------------------------------------
void AnimClip::CalculateTimeStamps()
{
	// start time
	m_startTimeMilliseconds = 0.f;

	// end time: go through all joints/channels, find the last end time
	m_endTimeMilliseconds = 0.f;

	for ( int jointId = 0; jointId < m_animChannels.size(); jointId++ )
	{
		AnimChannel const& animChannel				  = m_animChannels[ jointId ];
		float			   channelEndTimeMilliSeconds = animChannel.GetEndTimeMilliSeconds();
		if ( channelEndTimeMilliSeconds > m_endTimeMilliseconds )
		{
			m_endTimeMilliseconds = channelEndTimeMilliSeconds;
		}
	}
}


//----------------------------------------------------------------------------------------------------------
AnimClip* AnimClip::LoadOrGetAnimationClip( std::string clipFilePath )
{
	auto iter = s_animClipRegistery.find( clipFilePath );
	if ( iter != s_animClipRegistery.cend() )
	{
		return iter->second;
	}
	else // load from file
	{
		AnimClip* newClip = new AnimClip();
		FbxFileImporter::LoadAnimClipFromFile( clipFilePath.c_str(), *newClip );

		s_animClipRegistery[ clipFilePath ] = newClip;
		return newClip;
	}

	return nullptr;
}


//----------------------------------------------------------------------------------------------------------
Vec3AnimCurve& AnimClip::GetRootJointTranslationCurveByRefernce()
{
	unsigned int   rootJointId			= 0;
	AnimChannel&   rootJoint			= m_animChannels[ rootJointId ];
	Vec3AnimCurve& rootJointTranslation = rootJoint.m_positionCurve;

	return rootJointTranslation;
}


//----------------------------------------------------------------------------------------------------------
Vec3AnimCurve const& AnimClip::GetRootJointTranslationCurve()
{
	unsigned int		 rootJointId		  = 0;
	AnimChannel const&	 rootJoint			  = m_animChannels[ rootJointId ];
	Vec3AnimCurve const& rootJointTranslation = rootJoint.m_positionCurve;

	return rootJointTranslation;
}
