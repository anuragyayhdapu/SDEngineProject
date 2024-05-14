#pragma once

#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Mat44.hpp"


#include <vector>
#include <string>


//-------------------------------------------------------------------------
// The pose class needs to keep track of the transformation of every joint in
// the skeleton of the animating character.
// It also needs to keep track of the parent joint of every joint.
class AnimPose
{
public:
	AnimPose();

	static void Blend( AnimPose& outResultPose, AnimPose const& poseA, AnimPose const& poseB, float parametricZeroToOne, int rootJointId );

	// copy
	AnimPose( AnimPose const& copy );
	AnimPose& operator=( AnimPose const& copyFrom );

	void AddJoint( Transform const& localTransform, int parentIndex, std::string jointName);
	void SetLocalTransformOfJoint( Transform const& newLocalTransform, int jointId );

	int		  GetNumberOfJoints() const;
	int		  GetParentOfJoint( int jointId ) const;
	Transform GetLocalTransformOfJoint( int jointId ) const;
	Transform GetGlobalTransformOfJoint( int jointId ) const;

	bool IsJointUnderRootHierarchy( int jointId, int rootId ) const;

	// matrix
	void GetMatrixArray( std::vector<Mat44>& out ) const;

	// operators
	bool operator==( AnimPose const& other );
	bool operator!=( AnimPose const& other );

	void GetDifference( AnimPose const& poseToSubtract, AnimPose& outResultPose );
	void GetAddition( AnimPose const& poseToAdd, AnimPose& outResultPose );

	void CalculateGlobalInverseBindPoseMatrices();
	Mat44 const& GetGlobalInverseBindPoseMatrixOfJoint( int jointId ) const;
	std::vector<Mat44> m_jointGlobalInverseBindPoseMatrices;


protected:
	std::vector<Transform>	 m_jointLocalTransforms;
	std::vector<int>		 m_parentJointIndices;
	std::vector<std::string> m_jointNames;
};