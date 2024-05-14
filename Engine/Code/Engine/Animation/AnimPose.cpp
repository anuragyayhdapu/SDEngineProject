#include "Engine/Animation/AnimPose.hpp"
#include "Engine/Math/MathUtils.hpp"


//-------------------------------------------------------------------------
AnimPose::AnimPose()
{
}


//-------------------------------------------------------------------------
AnimPose::AnimPose( AnimPose const& copy )
{
	m_parentJointIndices   = copy.m_parentJointIndices;
	m_jointLocalTransforms = copy.m_jointLocalTransforms;
	m_jointNames		   = copy.m_jointNames;
}


//-------------------------------------------------------------------------
// TODO later: optimize later with fast copy
AnimPose& AnimPose::operator=( AnimPose const& copyFrom )
{
	if ( this == &copyFrom )
	{
		return *this;
	}

	m_parentJointIndices   = copyFrom.m_parentJointIndices;
	m_jointLocalTransforms = copyFrom.m_jointLocalTransforms;
	m_jointNames		   = copyFrom.m_jointNames;

	return *this;
}


//-------------------------------------------------------------------------
int AnimPose::GetNumberOfJoints() const
{
	return ( int ) m_jointLocalTransforms.size();
}


//-------------------------------------------------------------------------
int AnimPose::GetParentOfJoint( int jointId ) const
{
	return m_parentJointIndices[ jointId ];
}



//-------------------------------------------------------------------------
Transform AnimPose::GetLocalTransformOfJoint( int jointId ) const
{
	Transform localTransform = m_jointLocalTransforms[ jointId ];
	return localTransform;
}


//-------------------------------------------------------------------------
Transform AnimPose::GetGlobalTransformOfJoint( int jointId ) const
{
	Transform globalTransform = m_jointLocalTransforms[ jointId ];

	int parentIndex = m_parentJointIndices[ jointId ];
	while ( parentIndex >= 0 )
	{
		Transform&		 thisJointTransform	  = globalTransform;
		Transform const& parentJointTransform = m_jointLocalTransforms[ parentIndex ];

		globalTransform = Transform::ApplyChildToParentTransform( thisJointTransform, parentJointTransform );

		// move up
		int grandParentIndex = m_parentJointIndices[ parentIndex ];
		parentIndex			 = grandParentIndex;
	}

	return globalTransform;
}


//----------------------------------------------------------------------------------------------------------
void AnimPose::GetMatrixArray( std::vector<Mat44>& out ) const
{
	for ( int jointIndex = 0; jointIndex < m_jointLocalTransforms.size(); jointIndex++ )
	{
		Transform globalTransform = GetGlobalTransformOfJoint( jointIndex );
		Mat44	  matrix		  = Mat44::CreateFromTransform( globalTransform );
		out.push_back( matrix );
	}
}


//----------------------------------------------------------------------------------------------------------
void AnimPose::AddJoint( Transform const& localTransform, int parentIndex, std::string jointName)
{
	m_jointLocalTransforms.push_back( localTransform );
	m_parentJointIndices.push_back( parentIndex );
	m_jointNames.push_back( jointName );
}


//----------------------------------------------------------------------------------------------------------
void AnimPose::SetLocalTransformOfJoint( Transform const& newLocalTransform, int jointId )
{
	Transform& currentTransform = m_jointLocalTransforms[ jointId ];
	currentTransform.m_position = newLocalTransform.m_position;
	currentTransform.m_rotation = newLocalTransform.m_rotation;
	currentTransform.m_scale	= newLocalTransform.m_scale;
}


//-------------------------------------------------------------------------
bool AnimPose::operator==( AnimPose const& other )
{
	int mySize	  = GetNumberOfJoints();
	int otherSize = GetNumberOfJoints();
	if ( mySize != otherSize )
	{
		return false;
	}

	for ( int jointIndex = 0; jointIndex < mySize; jointIndex++ )
	{
		Transform myLocalTransform	  = GetLocalTransformOfJoint( jointIndex );
		Transform otherLocalTransform = other.GetLocalTransformOfJoint( jointIndex );

		if ( myLocalTransform != otherLocalTransform )
		{
			return false;
		}
	}

	return true;
}


//-------------------------------------------------------------------------
bool AnimPose::operator!=( AnimPose const& other )
{
	bool isSame = *this == other;
	return !isSame;
}


//----------------------------------------------------------------------------------------------------------
void AnimPose::Blend( AnimPose& outResultPose, AnimPose const& poseA, AnimPose const& poseB, float parametricZeroToOne, int blendRootJointId )
{
	int numJoints = poseA.GetNumberOfJoints();

	for ( int jointId = 0; jointId < numJoints; jointId++ )
	{
		if ( blendRootJointId >= 0 )
		{
			bool isJointUnderBlendRootJoint = poseA.IsJointUnderRootHierarchy( jointId, blendRootJointId );
			if ( isJointUnderBlendRootJoint )
			{
				Transform const& poseALocalTransform = poseA.GetLocalTransformOfJoint( jointId );
				Transform const& poseBLocalTransform = poseB.GetLocalTransformOfJoint( jointId );

				Transform blendedLocalTransform = LerpTransform( poseALocalTransform, poseBLocalTransform, parametricZeroToOne );
				outResultPose.SetLocalTransformOfJoint( blendedLocalTransform, jointId );
			}
			else // above blended root
			{
				continue;
			}
		}
		else // rootId = -1 is root of the entire skeletal
		{
			Transform const& poseALocalTransform = poseA.GetLocalTransformOfJoint( jointId );
			Transform const& poseBLocalTransform = poseB.GetLocalTransformOfJoint( jointId );

			Transform blendedLocalTransform = LerpTransform( poseALocalTransform, poseBLocalTransform, parametricZeroToOne );
			outResultPose.SetLocalTransformOfJoint( blendedLocalTransform, jointId );
		}
	}
}


//----------------------------------------------------------------------------------------------------------
bool AnimPose::IsJointUnderRootHierarchy( int jointId, int rootId ) const
{
	if ( jointId == rootId )
	{
		return true;
	}

	// bubble up and find root
	int parentJointId = m_parentJointIndices[ jointId ];
	while ( parentJointId >= 0 )
	{
		if ( parentJointId == rootId )
		{
			return true;
		}
		parentJointId = m_parentJointIndices[ parentJointId ];
	}

	return false;
}


//----------------------------------------------------------------------------------------------------------
// D = A - B
void AnimPose::GetDifference( AnimPose const& poseToSubtract, AnimPose& outResultPose )
{
	for ( int jointIndex = 0; jointIndex < m_jointLocalTransforms.size(); jointIndex++ )
	{
		Transform const& A = m_jointLocalTransforms[ jointIndex ];
		Transform const& B = poseToSubtract.m_jointLocalTransforms[ jointIndex ];
		Transform&		 D = outResultPose.m_jointLocalTransforms[ jointIndex ];

		// position
		D.m_position = A.m_position - B.m_position;

		// rotation
		Quaternion ARotationInverse	  = A.m_rotation.GetInverse();
		Quaternion rotationDifference = B.m_rotation * ARotationInverse;
		D.m_rotation				  = rotationDifference;

		// scale
		D.m_scale = A.m_scale - B.m_scale;
	}
}


//----------------------------------------------------------------------------------------------------------
void AnimPose::GetAddition( AnimPose const& poseToAdd, AnimPose& outResultPose )
{
	for ( int jointIndex = 0; jointIndex < m_jointLocalTransforms.size(); jointIndex++ )
	{
		Transform const& jointTransform		  = m_jointLocalTransforms[ jointIndex ];
		Transform const& jointTransformToAdd  = poseToAdd.m_jointLocalTransforms[ jointIndex ];
		Transform&		 resultJointTransform = outResultPose.m_jointLocalTransforms[ jointIndex ];

		resultJointTransform = jointTransform + jointTransformToAdd;
	}
}


//----------------------------------------------------------------------------------------------------------
void AnimPose::CalculateGlobalInverseBindPoseMatrices()
{
	for (int jointIndex = 0; jointIndex < m_jointLocalTransforms.size(); jointIndex++)
	{
		Transform jointGlobalTransform	 = GetGlobalTransformOfJoint( jointIndex );
		jointGlobalTransform.m_scale	 = Vec3( 1.f, 1.f, 1.f );
		Mat44 jointGlobalTransformMatrix = Mat44::CreateFromTransform( jointGlobalTransform );
		Mat44 jointGlobalInverseBindPoseMatrix = jointGlobalTransformMatrix.GetOrthonormalInverse();

		m_jointGlobalInverseBindPoseMatrices.push_back( jointGlobalInverseBindPoseMatrix );
	}
}


//----------------------------------------------------------------------------------------------------------
Mat44 const& AnimPose::GetGlobalInverseBindPoseMatrixOfJoint( int jointId ) const
{
	return m_jointGlobalInverseBindPoseMatrices[ jointId ];
}
