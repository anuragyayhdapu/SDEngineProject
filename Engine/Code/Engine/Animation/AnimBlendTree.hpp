#pragma once


class AnimBlendNode;
class AnimPose;


//----------------------------------------------------------------------------------------------------------
class AnimBlendTree
{
public:
	AnimBlendTree();
	~AnimBlendTree();

	AnimPose const Evaluate();


	AnimBlendNode* m_rootNode = nullptr;
};