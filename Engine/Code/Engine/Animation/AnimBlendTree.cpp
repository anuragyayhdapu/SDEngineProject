#include "Engine/Animation/AnimBlendTree.hpp"
#include "Engine/Animation/AnimBlendNode.hpp"



//----------------------------------------------------------------------------------------------------------
AnimBlendTree::AnimBlendTree()
{
}


//----------------------------------------------------------------------------------------------------------
AnimBlendTree::~AnimBlendTree()
{
	delete m_rootNode;
}


//----------------------------------------------------------------------------------------------------------
AnimPose const AnimBlendTree::Evaluate()
{
	return m_rootNode->Evaluate();
}
