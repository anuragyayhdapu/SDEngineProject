#pragma once

#include "Engine/Animation/AnimClip.hpp"
#include "Engine/Animation/AnimPose.hpp"
#include "Engine/Animation/Vertex_Skeletal.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"


//----------------------------------------------------------------------------------------------------------
class FbxFileImporter
{
public:
	FbxFileImporter( const char* file );
	~FbxFileImporter();

	AnimPose	m_restPose;
	AnimClip*	m_animClip = nullptr;

	static void LoadAnimClipFromFile( char const* filename, AnimClip& outClip );
	static void LoadRestPoseFromFile( char const* filename, AnimPose& outPose );
	static void LoadMeshFromFile( char const* filename, std::vector<Vertex_PCUTBN>& outVerts );
	static void LoadMeshFromFileIndexed( char const* filename, std::vector<Vertex_PCUTBN>& outVerts, std::vector<unsigned int>& outIndexes );
	static void LoadPreRiggedAndPreSkinnedMeshBindPoseFromFile( char const* filename, std::vector<Vertex_PCUTBN>& outverts, 
		std::vector<std::vector<std::pair<int, float>>>& outJointAndWeightsMapping);

protected:
	void		LoadRestPoseAndAnimationData( char const* file );
	char const* m_file = nullptr;
};
