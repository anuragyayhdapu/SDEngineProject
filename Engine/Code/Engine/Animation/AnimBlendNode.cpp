#include "Engine/Animation/AnimBlendNode.hpp"
#include "Engine/Animation/AnimClip.hpp"
#include "Engine/Animation/AnimPose.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"


//----------------------------------------------------------------------------------------------------------
AnimBlendNode::AnimBlendNode()
{
}


//----------------------------------------------------------------------------------------------------------
AnimBlendNode::~AnimBlendNode()
{
}


//----------------------------------------------------------------------------------------------------------
// AnimPoseNode
//----------------------------------------------------------------------------------------------------------
AnimPoseNode::AnimPoseNode( AnimPose const& animPose )
	: m_pose( animPose )
{
}


//----------------------------------------------------------------------------------------------------------
AnimPoseNode::~AnimPoseNode()
{
}


void AnimPoseNode::Update( float parametricZeroToOne )
{
	UNUSED( parametricZeroToOne );
}


//----------------------------------------------------------------------------------------------------------
AnimPose const AnimPoseNode::Evaluate()
{
	return m_pose;
}


//----------------------------------------------------------------------------------------------------------
void AnimPoseNode::AddChild( AnimBlendNode* childNode )
{
	UNUSED( childNode );
}


//----------------------------------------------------------------------------------------------------------
// BinaryLerpBlendNode
//----------------------------------------------------------------------------------------------------------
BinaryLerpBlendNode::BinaryLerpBlendNode()
{
}


//----------------------------------------------------------------------------------------------------------
BinaryLerpBlendNode::~BinaryLerpBlendNode()
{
	m_childNodeA = nullptr;
	m_childNodeB = nullptr;
}


//----------------------------------------------------------------------------------------------------------
void BinaryLerpBlendNode::Update( float parametricZeroToOne )
{
	m_blendValue = parametricZeroToOne;
}


//----------------------------------------------------------------------------------------------------------
AnimPose const BinaryLerpBlendNode::Evaluate()
{
	AnimPose const& poseA = m_childNodeA->Evaluate();
	AnimPose const& poseB = m_childNodeB->Evaluate();
	AnimPose::Blend( m_blendedPose, poseA, poseB, m_blendValue, -1 );

	return m_blendedPose;
}


//----------------------------------------------------------------------------------------------------------
void BinaryLerpBlendNode::AddChild( AnimBlendNode* childNode )
{
	UNUSED( childNode );
	/*
	 if ( m_childNodeA == nullptr )
	 {
		 m_childNodeA = childNode;
		 return;
	 }
	 else if ( m_childNodeB == nullptr )
	 {
		 m_childNodeB = childNode;
		 return;
	 }
	 else
	 {
		 DebuggerPrintf( "Warning: All child nodes assigned."
						 "Clear Tree before adding new children" );
	 }*/
}


//----------------------------------------------------------------------------------------------------------
void BinaryLerpBlendNode::ClearChildren()
{
	delete m_childNodeA;
	delete m_childNodeB;
}



//----------------------------------------------------------------------------------------------------------
// AnimClipNode
//----------------------------------------------------------------------------------------------------------
AnimClipNode::AnimClipNode( AnimClip const& animClip )
	: m_animClip( animClip )
{
}


//----------------------------------------------------------------------------------------------------------
AnimClipNode::~AnimClipNode()
{
}


//----------------------------------------------------------------------------------------------------------
void AnimClipNode::Update( float parametricZeroToOne )
{
	// convert normalized value to local time
	float clipDurationMs = m_animClip.GetEndTime() - m_animClip.GetStartTime();
	m_localTimeMs		 = parametricZeroToOne * clipDurationMs;

	if ( m_debug )
	{
		DebuggerPrintf( "name: %s, localTimeMs:%.2f\n", m_animClip.m_name.c_str(), m_localTimeMs );
	}
}


//----------------------------------------------------------------------------------------------------------
AnimPose const AnimClipNode::Evaluate()
{
	m_animClip.Sample( m_localTimeMs, m_sampledPose );

	return m_sampledPose;
}


//----------------------------------------------------------------------------------------------------------
void AnimClipNode::AddChild( AnimBlendNode* childNode )
{
	UNUSED( childNode );
}
