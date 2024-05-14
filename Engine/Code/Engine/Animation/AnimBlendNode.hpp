#pragma once


#include "Engine/Animation/AnimPose.hpp"


class AnimPose;
class AnimClip;


//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
class AnimBlendNode
{
public:
	AnimBlendNode();
	virtual ~AnimBlendNode() = 0;

	virtual void			Update( float parametricZeroToOne )	 = 0;
	virtual AnimPose const Evaluate()							 = 0;
	virtual void			AddChild( AnimBlendNode* childNode ) = 0;
	virtual void			ClearChildren()						 = 0;

	//----------------------------------------------------------------------------------------------------------
	bool m_debug = false;
};


//----------------------------------------------------------------------------------------------------------
class AnimPoseNode : public AnimBlendNode
{
public:
	AnimPoseNode( AnimPose const& animPose );
	~AnimPoseNode();

	AnimPose const& m_pose;

	virtual void			Update( float parametricZeroToOne );
	virtual AnimPose const Evaluate() override;
	virtual void			AddChild( AnimBlendNode* childNode ) override;
	virtual void			ClearChildren() override {}
};


//----------------------------------------------------------------------------------------------------------
class BinaryLerpBlendNode : public AnimBlendNode
{
public:
	BinaryLerpBlendNode();
	~BinaryLerpBlendNode();

	virtual void			Update( float parametricZeroToOne ) override;
	virtual AnimPose const Evaluate() override;
	virtual void			AddChild( AnimBlendNode* childNode ) override;
	virtual void			ClearChildren() override;

public:
	float	 m_blendValue = 0.f;
	AnimPose m_blendedPose;

//protected:
	AnimBlendNode* m_childNodeA = nullptr;
	AnimBlendNode* m_childNodeB = nullptr;
};


//----------------------------------------------------------------------------------------------------------
class AnimClipNode : public AnimBlendNode
{
public:
	AnimClipNode( AnimClip const& animClip );
	~AnimClipNode();

	virtual void			Update( float parametricZeroToOne ) override;
	virtual AnimPose const Evaluate() override;
	virtual void			AddChild( AnimBlendNode* childNode ) override;
	virtual void			ClearChildren() override {}

public:
	AnimClip const& m_animClip;
	AnimPose		m_sampledPose;
	float			m_localTimeMs = 0.f;
};


//----------------------------------------------------------------------------------------------------------
class AdditiveBlendNode : public AnimBlendNode
{
};