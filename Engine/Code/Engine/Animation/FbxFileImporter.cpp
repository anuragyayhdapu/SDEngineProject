#include "Engine/Animation/FbxFileImporter.hpp"
#include "Engine/Animation/AnimPose.hpp"
// #include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define FBXSDK_SHARED
#include "fbxsdk.h"

#include <queue>


//----------------------------------------------------------------------------------------------------------
float	   SCALE_OF_THE_FB_SCENE = 70.f;
//float	   SCALE_OF_THE_FB_SCENE = 1.f;

void	   AddJointNodeToPose( FbxNode* fbxNode, int parentIndex, AnimPose& pose );
void	   AddJointNodeAnimData( FbxNode* fbxNode, int jointId, /*FbxAnimStack* fbxAnimStack,*/ FbxAnimLayer* fbxAnimLayer, AnimClip* animClip );
FbxAMatrix CalculateFbxTransformMatrixOfFbxNode( FbxNode* fbxNode );
Rgba8	   GetDiffuseColorForPolygon( FbxLayerElementArrayTemplate<int>* materialIndices, int polygonIndex, FbxNode* fbxNode );
Vec3	   GetNormalFromControlPointOrVertexCount( FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter );
Vec3	   GetPositionOfControlPoint( FbxMesh* fbxMesh, int controlPointIndex, FbxAMatrix const& nodeTansformMatrix );
void	   DebugPrintFbxNodeAttributeType( FbxNodeAttribute::EType nodeAttributeType );
void	   GetJointNameToJointIdMap( FbxNode* fbxRootNode, std::map<std::string, int>& outRefMap );
void	   GetControlPointIndexToJointIdMap( FbxMesh* meshNode, std::map<std::string, int> const& jointNameToJointIdMap, std::map<int, std::vector<std::pair<int, float>>>& outRefMap );


//----------------------------------------------------------------------------------------------------------
FbxFileImporter::FbxFileImporter( const char* file )
	: m_file( file )
{
	LoadRestPoseAndAnimationData( file );
}


//----------------------------------------------------------------------------------------------------------
FbxFileImporter::~FbxFileImporter()
{
}


//----------------------------------------------------------------------------------------------------------
void FbxFileImporter::LoadAnimClipFromFile( char const* filename, AnimClip& outClip )
{
	// initialize fbx
	FbxManager* fbxManager = FbxManager::Create();

	// defines what to load from the fbx file, default is all the data
	FbxIOSettings* ioSettings = FbxIOSettings::Create( fbxManager, IOSROOT );
	fbxManager->SetIOSettings( ioSettings );

	// load file
	FbxImporter* importer	  = FbxImporter::Create( fbxManager, "Importer" );
	bool		 importStatus = importer->Initialize( filename, -1, fbxManager->GetIOSettings() );
	FbxScene*	 fbxScene	  = nullptr;

	if ( importStatus )
	{
		fbxScene = FbxScene::Create( fbxManager, "FbxScene" );
		importer->Import( fbxScene );
		importer->Destroy();
	}
	else // error occurred
	{
		DebuggerPrintf( "Call to FbxImporter::Initialize() Failed.\n" );
		DebuggerPrintf( "Error returned: %s\n\n", importer->GetStatus().GetErrorString() );
		return;
	}


	// 1. change the axis system in fbx scene
	FbxAxisSystem defaultAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem sdAxisSystem		= FbxAxisSystem( FbxAxisSystem::EUpVector::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded );
	if ( defaultAxisSystem != sdAxisSystem )
	{
		// fbxScene->GetGlobalSettings().SetAxisSystem( sdAxisSystem );

		// sdAxisSystem.ConvertScene( fbxScene );
		sdAxisSystem.DeepConvertScene( fbxScene );
	}

	FbxSystemUnit defaultUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	FbxSystemUnit sdUnit	  = FbxSystemUnit( SCALE_OF_THE_FB_SCENE );
	sdUnit.ConvertScene( fbxScene );


	// load the first animation stack
	int numFbxAnimStatcks = fbxScene->GetSrcObjectCount<FbxAnimStack>();
	if ( numFbxAnimStatcks <= 0 )
	{
		DebuggerPrintf( "No Anim Stack found.\n" );
		return;
	}
	

	// quick hack, remove later
	int numAnimStacks		   = fbxScene->GetMemberCount<FbxAnimStack>();
	int			  stackToGetAnimDataFrom = 0;
	if ( numAnimStacks == 2 )
		stackToGetAnimDataFrom = 1;

	FbxAnimStack* fbxAnimStack = fbxScene->GetSrcObject<FbxAnimStack>( stackToGetAnimDataFrom );

	// get the first animation layer
	int numFbxAnimLayers = fbxAnimStack->GetMemberCount<FbxAnimLayer>();
	if ( numFbxAnimLayers <= 0 )
	{
		DebuggerPrintf( "No Anim Layer found.\n" );
		return;
	}
	FbxAnimLayer* fbxAnimlayer = fbxAnimStack->GetMember<FbxAnimLayer>();


	outClip.m_name			   = filename;


	// 2. get the skeleton root node
	FbxNode* rootNode	= fbxScene->GetRootNode();
	int		 childCount = rootNode->GetChildCount();

	struct Joint
	{
		int		 m_jointId	   = -1;
		int		 m_parentIndex = -1;
		FbxNode* m_fbxNode	   = nullptr;
	};
	std::queue<Joint> jointQueue;

	// root skeleton joint
	for ( int childIndex = 0; childIndex < childCount; childIndex++ )
	{
		FbxNode* childNode		= rootNode->GetChild( childIndex );
		bool	 isSkeletonNode = childNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton;
		if ( isSkeletonNode )
		{
			jointQueue.push( { 0, -1, childNode } );

			break;
		}
	}

	AnimPose pose;

	// child skeletal joints
	while ( !jointQueue.empty() )
	{
		// skeletal data
		Joint& joint = jointQueue.front();
		AddJointNodeToPose( joint.m_fbxNode, joint.m_parentIndex, pose );
		joint.m_jointId = pose.GetNumberOfJoints() - 1;

		// animation data
		AddJointNodeAnimData( joint.m_fbxNode, joint.m_jointId, fbxAnimlayer, &outClip );


		for ( int index = 0; index < joint.m_fbxNode->GetChildCount(); index++ )
		{
			FbxNode* childNode = joint.m_fbxNode->GetChild( index );

			Joint	 newJoint;
			newJoint.m_fbxNode	   = childNode;
			newJoint.m_parentIndex = joint.m_jointId;

			jointQueue.push( newJoint );
		}

		jointQueue.pop();
	}

	outClip.CalculateTimeStamps();

	fbxScene->Destroy();
	fbxManager->Destroy();
}


//----------------------------------------------------------------------------------------------------------
void FbxFileImporter::LoadRestPoseFromFile( char const* filename, AnimPose& outPose )
{
	// initialize fbx
	FbxManager* fbxManager = FbxManager::Create();

	// defines what to load from the fbx file, default is all the data
	FbxIOSettings* ioSettings = FbxIOSettings::Create( fbxManager, IOSROOT );
	fbxManager->SetIOSettings( ioSettings );

	// load file
	FbxImporter* importer	  = FbxImporter::Create( fbxManager, "Importer" );
	bool		 importStatus = importer->Initialize( filename, -1, fbxManager->GetIOSettings() );
	FbxScene*	 fbxScene	  = nullptr;
	if ( importStatus )
	{
		fbxScene = FbxScene::Create( fbxManager, "FbxScene" );
		importer->Import( fbxScene );
		importer->Destroy();
	}
	else // error occurred
	{
		DebuggerPrintf( "Call to FbxImporter::Initialize() Failed.\n" );
		DebuggerPrintf( "Error returned: %s\n\n", importer->GetStatus().GetErrorString() );
		return;
	}

	// change the axis system in fbx scene
	FbxAxisSystem defaultAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem sdAxisSystem		= FbxAxisSystem( FbxAxisSystem::EUpVector::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded );
	if ( defaultAxisSystem != sdAxisSystem )
	{
		sdAxisSystem.DeepConvertScene( fbxScene );
	}

	//// change scale of the fbx scene
	FbxSystemUnit defaultUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	FbxSystemUnit sdUnit	  = FbxSystemUnit( SCALE_OF_THE_FB_SCENE );
	sdUnit.ConvertScene( fbxScene );

	// get the skeleton root node
	FbxNode* rootNode	= fbxScene->GetRootNode();
	int		 childCount = rootNode->GetChildCount();
	struct Joint
	{
		int		 m_jointId	   = -1;
		int		 m_parentIndex = -1;
		FbxNode* m_fbxNode	   = nullptr;
	};
	std::queue<Joint> jointQueue;
	for ( int childIndex = 0; childIndex < childCount; childIndex++ )
	{
		FbxNode*				childNode		  = rootNode->GetChild( childIndex );
		FbxNodeAttribute::EType nodeAttributeType = childNode->GetNodeAttribute()->GetAttributeType();
		DebugPrintFbxNodeAttributeType( nodeAttributeType );

		bool isSkeletonNode = childNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton;
		if ( isSkeletonNode )
		{
			jointQueue.push( { 0, -1, childNode } );
			break;
		}
	}

	// child skeletal joints
	while ( !jointQueue.empty() )
	{
		// skeletal data
		Joint& joint = jointQueue.front();
		AddJointNodeToPose( joint.m_fbxNode, joint.m_parentIndex, outPose );
		joint.m_jointId = outPose.GetNumberOfJoints() - 1;

		for ( int index = 0; index < joint.m_fbxNode->GetChildCount(); index++ )
		{
			FbxNode* childNode = joint.m_fbxNode->GetChild( index );

			Joint	 newJoint;
			newJoint.m_fbxNode	   = childNode;
			newJoint.m_parentIndex = joint.m_jointId;

			jointQueue.push( newJoint );
		}

		jointQueue.pop();
	}

	fbxScene->Destroy();
	fbxManager->Destroy();
}


//----------------------------------------------------------------------------------------------------------
void DebugPrintFbxNodeAttributeType( FbxNodeAttribute::EType nodeAttributeType )
{
	switch ( nodeAttributeType )
	{
	case fbxsdk::FbxNodeAttribute::eUnknown:
		DebuggerPrintf( "FbxNodeAttribute::eUnknown\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eNull:
		DebuggerPrintf( "FbxNodeAttribute::eNull\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eMarker:
		DebuggerPrintf( "FbxNodeAttribute::eMarker\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eSkeleton:
		DebuggerPrintf( "FbxNodeAttribute::eSkeleton\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eMesh:
		DebuggerPrintf( "FbxNodeAttribute::eMesh\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eNurbs:
		DebuggerPrintf( "FbxNodeAttribute::eNurbs\n" );
		break;
	case fbxsdk::FbxNodeAttribute::ePatch:
		DebuggerPrintf( "FbxNodeAttribute::ePatch\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eCamera:
		DebuggerPrintf( "FbxNodeAttribute::eCamera\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eCameraStereo:
		DebuggerPrintf( "FbxNodeAttribute::eCameraStereo\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eCameraSwitcher:
		DebuggerPrintf( "FbxNodeAttribute::eCameraSwitcher\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eLight:
		DebuggerPrintf( "FbxNodeAttribute::eLight\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eOpticalReference:
		DebuggerPrintf( "FbxNodeAttribute::eOpticalReference\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eOpticalMarker:
		DebuggerPrintf( "FbxNodeAttribute::eOpticalMarker\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eNurbsCurve:
		DebuggerPrintf( "FbxNodeAttribute::eNurbsCurve\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eTrimNurbsSurface:
		DebuggerPrintf( "FbxNodeAttribute::eTrimNurbsSurface\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eBoundary:
		DebuggerPrintf( "FbxNodeAttribute::eBoundary\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eNurbsSurface:
		DebuggerPrintf( "FbxNodeAttribute::eNurbsSurface\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eShape:
		DebuggerPrintf( "FbxNodeAttribute::eShape\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eLODGroup:
		DebuggerPrintf( "FbxNodeAttribute::eLODGroup\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eSubDiv:
		DebuggerPrintf( "FbxNodeAttribute::eSubDiv\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eCachedEffect:
		DebuggerPrintf( "FbxNodeAttribute::eCachedEffect\n" );
		break;
	case fbxsdk::FbxNodeAttribute::eLine:
		DebuggerPrintf( "FbxNodeAttribute::eLine\n" );
		break;
	default:
		DebuggerPrintf( "Unknown Fbx Node Type\n" );
		break;
	}
}


//----------------------------------------------------------------------------------------------------------
void FbxFileImporter::LoadRestPoseAndAnimationData( char const* file )
{
	// initialize fbx
	FbxManager* fbxManager = FbxManager::Create();

	// defines what to load from the fbx file, default is all the data
	FbxIOSettings* ioSettings = FbxIOSettings::Create( fbxManager, IOSROOT );
	fbxManager->SetIOSettings( ioSettings );

	// load file
	FbxImporter* importer	  = FbxImporter::Create( fbxManager, "Importer" );
	bool		 importStatus = importer->Initialize( file, -1, fbxManager->GetIOSettings() );
	FbxScene*	 fbxScene	  = nullptr;
	if ( importStatus )
	{
		fbxScene = FbxScene::Create( fbxManager, "FbxScene" );
		importer->Import( fbxScene );
		importer->Destroy();
	}
	else // error occurred
	{
		DebuggerPrintf( "Call to FbxImporter::Initialize() Failed.\n" );
		DebuggerPrintf( "Error returned: %s\n\n", importer->GetStatus().GetErrorString() );
		return;
	}


	// change the axis system in fbx scene
	FbxAxisSystem defaultAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem sdAxisSystem		= FbxAxisSystem( FbxAxisSystem::EUpVector::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded );
	if ( defaultAxisSystem != sdAxisSystem )
	{
		sdAxisSystem.DeepConvertScene( fbxScene );
	}

	// change the scale of fbx scene
	FbxSystemUnit defaultUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	FbxSystemUnit sdUnit	  = FbxSystemUnit( SCALE_OF_THE_FB_SCENE );
	sdUnit.ConvertScene( fbxScene );

	// load the first animation stack
	int numFbxAnimStatcks = fbxScene->GetSrcObjectCount<FbxAnimStack>();
	if ( numFbxAnimStatcks <= 0 )
	{
		DebuggerPrintf( "No Anim Stack found.\n" );
		return;
	}
	FbxAnimStack* fbxAnimStack = fbxScene->GetSrcObject<FbxAnimStack>();

	// get the first animation layer
	int numFbxAnimLayers = fbxAnimStack->GetMemberCount<FbxAnimLayer>();
	if ( numFbxAnimLayers <= 0 )
	{
		DebuggerPrintf( "No Anim Layer found.\n" );
		return;
	}
	FbxAnimLayer* fbxAnimlayer = fbxAnimStack->GetMember<FbxAnimLayer>();
	m_animClip				   = new AnimClip();
	m_animClip->m_name		   = file;


	// get the skeleton root node
	FbxNode* rootNode	= fbxScene->GetRootNode();
	int		 childCount = rootNode->GetChildCount();
	struct Joint
	{
		int		 m_jointId	   = -1;
		int		 m_parentIndex = -1;
		FbxNode* m_fbxNode	   = nullptr;
	};
	std::queue<Joint> jointQueue;
	for ( int childIndex = 0; childIndex < childCount; childIndex++ )
	{
		FbxNode* childNode		= rootNode->GetChild( childIndex );
		bool	 isSkeletonNode = childNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton;
		if ( isSkeletonNode )
		{
			jointQueue.push( { 0, -1, childNode } );
			break;
		}
	}

	// child skeletal joints
	while ( !jointQueue.empty() )
	{
		// skeletal joint data
		Joint& joint = jointQueue.front();
		AddJointNodeToPose( joint.m_fbxNode, joint.m_parentIndex, m_restPose );
		joint.m_jointId = m_restPose.GetNumberOfJoints() - 1;

		// animation data
		AddJointNodeAnimData( joint.m_fbxNode, joint.m_jointId, fbxAnimlayer, m_animClip );

		for ( int index = 0; index < joint.m_fbxNode->GetChildCount(); index++ )
		{
			FbxNode* childNode = joint.m_fbxNode->GetChild( index );

			Joint	 newJoint;
			newJoint.m_fbxNode	   = childNode;
			newJoint.m_parentIndex = joint.m_jointId;

			jointQueue.push( newJoint );
		}

		jointQueue.pop();
	}

	m_animClip->CalculateTimeStamps();

	fbxScene->Destroy();
	fbxManager->Destroy();
}


//----------------------------------------------------------------------------------------------------------
void AddJointNodeToPose( FbxNode* fbxNode, int parentIndex, AnimPose& pose )
{
	FbxAMatrix	  fbxTransform			= fbxNode->EvaluateLocalTransform();
	FbxVector4	  fbxPosition			= fbxTransform.GetT();
	FbxQuaternion fbxQuaternionRotation = fbxTransform.GetQ();
	FbxVector4	  fbxScale				= fbxTransform.GetS();

	Transform	  localTransform;
	localTransform.m_position.x = ( float ) fbxPosition[ 0 ];
	localTransform.m_position.y = ( float ) fbxPosition[ 1 ];
	localTransform.m_position.z = ( float ) fbxPosition[ 2 ];

	localTransform.m_rotation.x = ( float ) fbxQuaternionRotation[ 0 ];
	localTransform.m_rotation.y = ( float ) fbxQuaternionRotation[ 1 ];
	localTransform.m_rotation.z = ( float ) fbxQuaternionRotation[ 2 ];
	localTransform.m_rotation.w = ( float ) fbxQuaternionRotation[ 3 ];

	localTransform.m_scale.x	= ( float ) fbxScale[ 0 ];
	localTransform.m_scale.y	= ( float ) fbxScale[ 1 ];
	localTransform.m_scale.z	= ( float ) fbxScale[ 2 ];

	pose.AddJoint( localTransform, parentIndex, fbxNode->GetName());
}


//----------------------------------------------------------------------------------------------------------
void AddJointNodeAnimData( FbxNode* fbxNode, int jointId, /*FbxAnimStack* fbxAnimStack, */ FbxAnimLayer* fbxAnimLayer, AnimClip* animClip )
{
	AnimChannel jointAnimChannel;
	jointAnimChannel.m_jointId	 = jointId;
	jointAnimChannel.m_jointName = fbxNode->GetName();

	// keyframe data for local translation
	FbxAnimCurve* animCurveTx = fbxNode->LclTranslation.GetCurve( fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X );
	if ( animCurveTx )
	{
		int			  numKeysTranslation = animCurveTx->KeyGetCount();

		Vec3AnimCurve positionAnimCurve;
		for ( int index = 0; index < numKeysTranslation; index++ )
		{
			FbxTime			fbxTime			 = animCurveTx->KeyGetTime( index );
			float			timeMilliSeconds = static_cast<float>( fbxTime.GetMilliSeconds() );
			FbxAMatrix		fbxTransform	 = fbxNode->EvaluateLocalTransform( fbxTime );
			FbxVector4		fbxPosition		 = fbxTransform.GetT();

			Vector3Keyframe vec3KeyFrame;
			vec3KeyFrame.m_timeMilliSeconds = timeMilliSeconds;
			vec3KeyFrame.m_value			= Vec3( ( float ) fbxPosition[ 0 ], ( float ) fbxPosition[ 1 ], ( float ) fbxPosition[ 2 ] );
			positionAnimCurve.m_keyframes.push_back( vec3KeyFrame );
		}
		jointAnimChannel.m_positionCurve = positionAnimCurve;
	}

	// keyframe data for local rotation
	FbxAnimCurve* animCurveRx = fbxNode->LclRotation.GetCurve( fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X );
	if ( animCurveRx )
	{
		int					numKeysRotation = animCurveRx->KeyGetCount();
		QuaternionAnimCurve rotationAnimCurve;
		for ( int index = 0; index < numKeysRotation; index++ )
		{
			FbxTime			   fbxTime			= animCurveRx->KeyGetTime( index );
			float			   timeMilliSeconds = static_cast<float>( fbxTime.GetMilliSeconds() );
			FbxAMatrix		   fbxTransform		= fbxNode->EvaluateLocalTransform( fbxTime );
			FbxQuaternion	   fbxRotation		= fbxTransform.GetQ();

			QuaternionKeyframe quaternionKeyframe;
			quaternionKeyframe.m_timeMilliSeconds = timeMilliSeconds;
			quaternionKeyframe.m_value			  = Quaternion( ( float ) fbxRotation[ 0 ], ( float ) fbxRotation[ 1 ], ( float ) fbxRotation[ 2 ], ( float ) fbxRotation[ 3 ] );
			rotationAnimCurve.m_keyframes.push_back( quaternionKeyframe );
		}
		jointAnimChannel.m_rotationCurve = rotationAnimCurve;
	}

	// keyframe data for local scale
	FbxAnimCurve* animCurveSx = fbxNode->LclScaling.GetCurve( fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X );
	if ( animCurveSx )
	{
		int			  numKeysScale = animCurveSx->KeyGetCount();

		Vec3AnimCurve scaleAnimCurve;
		for ( int index = 0; index < numKeysScale; index++ )
		{
			FbxTime			fbxTime			 = animCurveSx->KeyGetTime( index );
			float			timeMilliSeconds = static_cast<float>( fbxTime.GetMilliSeconds() );
			FbxAMatrix		fbxTransform	 = fbxNode->EvaluateLocalTransform( fbxTime );
			FbxVector4		fbxScale		 = fbxTransform.GetS();

			Vector3Keyframe scaleKeyframe;
			scaleKeyframe.m_timeMilliSeconds = timeMilliSeconds;
			scaleKeyframe.m_value			 = Vec3( ( float ) fbxScale[ 0 ], ( float ) fbxScale[ 1 ], ( float ) fbxScale[ 2 ] );
			scaleAnimCurve.m_keyframes.push_back( scaleKeyframe );
		}
		jointAnimChannel.m_scaleCurve = scaleAnimCurve;
	}

	// add to clip
	animClip->m_animChannels.push_back( jointAnimChannel );
}


//----------------------------------------------------------------------------------------------------------
void FbxFileImporter::LoadMeshFromFile( char const* filename, std::vector<Vertex_PCUTBN>& outVerts )
{
	// initialize fbx
	FbxManager* fbxManager = FbxManager::Create();

	// defines what to load from the fbx file, default is all the data
	FbxIOSettings* ioSettings = FbxIOSettings::Create( fbxManager, IOSN_GEOMETRY );
	fbxManager->SetIOSettings( ioSettings );

	// load file
	FbxImporter* importer	  = FbxImporter::Create( fbxManager, "Importer" );
	bool		 importStatus = importer->Initialize( filename, -1, fbxManager->GetIOSettings() );
	FbxScene*	 fbxScene	  = nullptr;
	if ( importStatus )
	{
		fbxScene = FbxScene::Create( fbxManager, "FbxScene" );
		importer->Import( fbxScene );
		importer->Destroy();
	}
	else // error occurred
	{
		DebuggerPrintf( "Call to FbxImporter::Initialize() Failed.\n" );
		DebuggerPrintf( "Error returned: %s\n\n", importer->GetStatus().GetErrorString() );
		return;
	}

	// change the axis system in fbx scene
	FbxAxisSystem defaultAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem sdAxisSystem		= FbxAxisSystem( FbxAxisSystem::EUpVector::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded );
	if ( defaultAxisSystem != sdAxisSystem )
	{
		sdAxisSystem.DeepConvertScene( fbxScene );
	}

	// change the unit in the fbx scene
	FbxSystemUnit defaultUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	FbxSystemUnit sdUnit	  = FbxSystemUnit( SCALE_OF_THE_FB_SCENE );
	sdUnit.ConvertScene( fbxScene );

	// triangulate the scene
	FbxGeometryConverter fbxGeometryConverter( fbxManager );
	fbxGeometryConverter.Triangulate( fbxScene, true );

	// process mesh data and fill up our verts
	FbxNode* rootNode	= fbxScene->GetRootNode();
	int		 childCount = rootNode->GetChildCount();
	for ( int rootNodeChildIndex = 0; rootNodeChildIndex < childCount; rootNodeChildIndex++ )
	{
		FbxNode* immidiateChildNode = rootNode->GetChild( rootNodeChildIndex );
		bool	 isMeshNode			= immidiateChildNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh;
		if ( isMeshNode )
		{
			// cast this node to mesh node
			FbxMesh* meshNode = immidiateChildNode->GetMesh();

			// get all material indices of this mesh
			FbxLayerElementArrayTemplate<int>* materialIndices = nullptr;
			meshNode->GetMaterialIndices( &materialIndices );

			// calculate transform matrix of each mesh
			FbxAMatrix nodeTansformMatrix = CalculateFbxTransformMatrixOfFbxNode( immidiateChildNode );

			// for each triangle(polygon) in the mesh
			int triangleCount = meshNode->GetPolygonCount();
			int vertexCounter = 0;
			for ( int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++ )
			{
				// get the diffuse color for this triangle(polygon) from material
				Rgba8 diffuseColor = GetDiffuseColorForPolygon( materialIndices, triangleIndex, immidiateChildNode );

				// for each position in the triangle(polygon)
				int numPositionsInTriangle = meshNode->GetPolygonSize( triangleIndex );
				GUARANTEE_OR_DIE( numPositionsInTriangle == 3, "Polygon not a triangle in Fbx model" );
				for ( int positionInTriangle = 0; positionInTriangle < numPositionsInTriangle; positionInTriangle++ )
				{
					// get the index (this is equivalent to index in the index buffer)
					int controlPointIndex = meshNode->GetPolygonVertex( triangleIndex, positionInTriangle );

					// get the vertex (this is vertex in vertex buffer or vertex array)
					Vec3 position = GetPositionOfControlPoint( meshNode, controlPointIndex, nodeTansformMatrix );

					// get normal
					Vec3 normal = GetNormalFromControlPointOrVertexCount( meshNode, controlPointIndex, vertexCounter );

					// finally create the 'SD Engine Vertex'
					Vertex_PCUTBN vertex;
					vertex.m_position = position;
					vertex.m_color	  = diffuseColor;
					vertex.m_normal	  = normal;
					outVerts.push_back( vertex );

					vertexCounter += 1;
				}
			}
		}
	}

	// destroy and release fbx memory before exiting
	fbxScene->Destroy();
	fbxManager->Destroy();
}


//----------------------------------------------------------------------------------------------------------
// Known issue: vertex normals may not be correct if normal data is stored per vertex instead of per index
void FbxFileImporter::LoadMeshFromFileIndexed( char const* filename, std::vector<Vertex_PCUTBN>& outVerts, std::vector<unsigned int>& outIndexes )
{
	// initialize fbx
	FbxManager* fbxManager = FbxManager::Create();

	// defines what to load from the fbx file, default is all the data
	FbxIOSettings* ioSettings = FbxIOSettings::Create( fbxManager, IOSN_GEOMETRY );
	fbxManager->SetIOSettings( ioSettings );

	// load file
	FbxImporter* importer	  = FbxImporter::Create( fbxManager, "Importer" );
	bool		 importStatus = importer->Initialize( filename, -1, fbxManager->GetIOSettings() );
	FbxScene*	 fbxScene	  = nullptr;
	if ( importStatus )
	{
		fbxScene = FbxScene::Create( fbxManager, "FbxScene" );
		importer->Import( fbxScene );
		importer->Destroy();
	}
	else // error occurred
	{
		DebuggerPrintf( "Call to FbxImporter::Initialize() Failed.\n" );
		DebuggerPrintf( "Error returned: %s\n\n", importer->GetStatus().GetErrorString() );
		return;
	}

	// change the axis system in fbx scene
	FbxAxisSystem defaultAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem sdAxisSystem		= FbxAxisSystem( FbxAxisSystem::EUpVector::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded );
	if ( defaultAxisSystem != sdAxisSystem )
	{
		sdAxisSystem.DeepConvertScene( fbxScene );
	}

	// change the unit in the fbx scene
	FbxSystemUnit defaultUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	FbxSystemUnit sdUnit	  = FbxSystemUnit( SCALE_OF_THE_FB_SCENE );
	sdUnit.ConvertScene( fbxScene );

	// triangulate the scene
	FbxGeometryConverter fbxGeometryConverter( fbxManager );
	fbxGeometryConverter.Triangulate( fbxScene, true );

	// process mesh data and fill up our verts
	FbxNode* rootNode	= fbxScene->GetRootNode();
	int		 childCount = rootNode->GetChildCount();
	for ( int rootNodeChildIndex = 0; rootNodeChildIndex < childCount; rootNodeChildIndex++ )
	{
		FbxNode* immidiateChildNode = rootNode->GetChild( rootNodeChildIndex );
		bool	 isMeshNode			= immidiateChildNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh;
		if ( isMeshNode )
		{
			// cast this node to mesh node
			FbxMesh* meshNode = immidiateChildNode->GetMesh();

			// get all material indices of this mesh
			FbxLayerElementArrayTemplate<int>* materialIndices = nullptr;
			meshNode->GetMaterialIndices( &materialIndices );

			// calculate transform matrix of each mesh
			FbxAMatrix nodeTansformMatrix = CalculateFbxTransformMatrixOfFbxNode( immidiateChildNode );

			// calculate indexes
			// for each triangle(polygon) in the mesh
			int triangleCount = meshNode->GetPolygonCount();
			for ( int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++ )
			{
				// get the diffuse color for this triangle(polygon) from material
				Rgba8 diffuseColor = GetDiffuseColorForPolygon( materialIndices, triangleIndex, immidiateChildNode );

				// for each position in the triangle(polygon)
				int numPositionsInTriangle = meshNode->GetPolygonSize( triangleIndex );
				GUARANTEE_OR_DIE( numPositionsInTriangle == 3, "Polygon not a triangle in Fbx model" );
				for ( int positionInTriangle = 0; positionInTriangle < numPositionsInTriangle; positionInTriangle++ )
				{
					// get the index (this is equivalent to index in the index buffer)
					int controlPointIndexInThisMesh = meshNode->GetPolygonVertex( triangleIndex, positionInTriangle );
					int totalNumVertexes			= static_cast<int>( outVerts.size() );
					int controlPointIndex			= totalNumVertexes + controlPointIndexInThisMesh;
					outIndexes.push_back( controlPointIndex );
				}
			}

			// calculate vertexes
			int numControlPoints = meshNode->GetControlPointsCount();
			int vertexCounter	 = 0;
			for ( int controlPointIndex = 0; controlPointIndex < numControlPoints; controlPointIndex++ )
			{
				// get the vertex (this is vertex in vertex buffer or vertex array)
				Vec3 position = GetPositionOfControlPoint( meshNode, controlPointIndex, nodeTansformMatrix );

				// get normal
				Vec3 normal = GetNormalFromControlPointOrVertexCount( meshNode, controlPointIndex, vertexCounter );

				// finally create the 'SD Engine Vertex'
				Vertex_PCUTBN vertex;
				vertex.m_position = position;
				// vertex.m_color	  = diffuseColor;
				vertex.m_normal = normal;
				outVerts.push_back( vertex );

				vertexCounter += 1;
			}

			// TODO: remove later
			/*if ( rootNodeChildIndex >= 5 )
				break;*/
		}
	}

	// destroy and release fbx memory before exiting
	fbxScene->Destroy();
	fbxManager->Destroy();
}


//----------------------------------------------------------------------------------------------------------
FbxAMatrix CalculateFbxTransformMatrixOfFbxNode( FbxNode* fbxNode )
{
	FbxAMatrix localTransformMatrix = fbxNode->EvaluateGlobalTransform();

	FbxAMatrix geometricMatrix;
	geometricMatrix.SetIdentity();
	if ( fbxNode->GetNodeAttribute() )
	{
		FbxVector4 geoT = fbxNode->GetGeometricTranslation( FbxNode::eSourcePivot );
		FbxVector4 geoR = fbxNode->GetGeometricRotation( FbxNode::eSourcePivot );
		FbxVector4 geoS = fbxNode->GetGeometricScaling( FbxNode::eSourcePivot );
		geometricMatrix.SetT( geoT );
		geometricMatrix.SetR( geoR );
		geometricMatrix.SetS( geoS );
	}

	FbxAMatrix finalTransformMatrix = localTransformMatrix * geometricMatrix;
	return finalTransformMatrix;
}


//----------------------------------------------------------------------------------------------------------
Rgba8 GetDiffuseColorForPolygon( FbxLayerElementArrayTemplate<int>* materialIndices, int polygonIndex, FbxNode* fbxNode )
{
	if ( materialIndices == nullptr )
	{
		Rgba8 whiteColor = Rgba8::WHITE;
		return whiteColor;
	}

	int					materialIndex = materialIndices->GetAt( polygonIndex );
	FbxSurfaceMaterial* material	  = fbxNode->GetMaterial( materialIndex );

	FbxDouble3			fbxColor( 1.0, 0.0, 0.0 );
	if ( material->GetClassId().Is( FbxSurfacePhong::ClassId ) )
	{
		FbxSurfacePhong* phongMaterial = static_cast<FbxSurfacePhong*>( material );
		fbxColor					   = phongMaterial->Diffuse;
	}
	else if ( material->GetClassId().Is( FbxSurfaceLambert::ClassId ) )
	{
		FbxSurfaceLambert* lambertMaterial = static_cast<FbxSurfacePhong*>( material );
		fbxColor						   = lambertMaterial->Diffuse;
	}

	Rgba8 diffuseColor = Rgba8::GetFromFloats( ( float ) fbxColor[ 0 ], ( float ) fbxColor[ 1 ], ( float ) fbxColor[ 2 ] );
	return diffuseColor;
}


//----------------------------------------------------------------------------------------------------------
Vec3 GetPositionOfControlPoint( FbxMesh* fbxMesh, int controlPointIndex, FbxAMatrix const& nodeTansformMatrix )
{
	FbxVector4 fbxControlPoint = fbxMesh->GetControlPointAt( controlPointIndex );
	fbxControlPoint			   = nodeTansformMatrix.MultT( fbxControlPoint );

	Vec3 position;
	position.x = static_cast<float>( fbxControlPoint[ 0 ] );
	position.y = static_cast<float>( fbxControlPoint[ 1 ] );
	position.z = static_cast<float>( fbxControlPoint[ 2 ] );
	return position;
}


//----------------------------------------------------------------------------------------------------------
// For Reference: https://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_cpp_ref_normals_2main_8cxx_example_html
// & https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582/#google_vignette
Vec3 GetNormalFromControlPointOrVertexCount( FbxMesh* fbxMesh, int controlPointIndex, int vertexCounter )
{
	Vec3					  normal		= Vec3::ZERO;
	FbxGeometryElementNormal* normalElement = fbxMesh->GetElementNormal();
	if ( !normalElement )
	{
		return normal;
	}

	int normalMappingMode  = normalElement->GetMappingMode();
	int normalRefernceMode = normalElement->GetReferenceMode();
	switch ( normalMappingMode )
	{
	case FbxGeometryElement::eByControlPoint: {
		switch ( normalRefernceMode )
		{
		case FbxGeometryElement::eDirect: {
			FbxVector4 fbxNormal = normalElement->GetDirectArray().GetAt( controlPointIndex );
			normal.x			 = static_cast<float>( fbxNormal[ 0 ] );
			normal.y			 = static_cast<float>( fbxNormal[ 1 ] );
			normal.z			 = static_cast<float>( fbxNormal[ 2 ] );

			break;
		}
		case FbxGeometryElement::eIndexToDirect: {
			int		   normalIndex = normalElement->GetIndexArray().GetAt( controlPointIndex );
			FbxVector4 fbxNormal   = normalElement->GetDirectArray().GetAt( normalIndex );
			normal.x			   = static_cast<float>( fbxNormal[ 0 ] );
			normal.y			   = static_cast<float>( fbxNormal[ 1 ] );
			normal.z			   = static_cast<float>( fbxNormal[ 2 ] );

			break;
		}
		default:
			ERROR_RECOVERABLE( "Error in Calculating Normal from Fbx File. Invalid Normal Refernce" );
			break;
		}

		break;
	}
	case FbxGeometryElement::eByPolygonVertex: {
		switch ( normalRefernceMode )
		{
		case FbxGeometryElement::eDirect: {
			FbxVector4 fbxNormal = normalElement->GetDirectArray().GetAt( vertexCounter );
			normal.x			 = static_cast<float>( fbxNormal[ 0 ] );
			normal.y			 = static_cast<float>( fbxNormal[ 1 ] );
			normal.z			 = static_cast<float>( fbxNormal[ 2 ] );

			break;
		}
		case FbxGeometryElement::eIndexToDirect: {
			int		   normalIndex = normalElement->GetIndexArray().GetAt( vertexCounter );
			FbxVector4 fbxNormal   = normalElement->GetDirectArray().GetAt( normalIndex );
			normal.x			   = static_cast<float>( fbxNormal[ 0 ] );
			normal.y			   = static_cast<float>( fbxNormal[ 1 ] );
			normal.z			   = static_cast<float>( fbxNormal[ 2 ] );

			break;
		}
		default:
			ERROR_RECOVERABLE( "Error in Calculating Normal from Fbx File. Invalid Normal Refernce." );
			break;
		}

		break;
	}
	default:
		ERROR_RECOVERABLE( "Error in Calculating Normal from Fbx File. Invalid Mapping Mode." );
		break;
	}

	return normal;
}


//----------------------------------------------------------------------------------------------------------
void FbxFileImporter::LoadPreRiggedAndPreSkinnedMeshBindPoseFromFile( char const* filename, std::vector<Vertex_PCUTBN>& outVerts, 
	std::vector<std::vector<std::pair<int, float>>>& outJointAndWeightsMapping)
{
	// initialize fbx
	FbxManager* fbxManager = FbxManager::Create();

	// defines what to load from the fbx file, default is all the data
	FbxIOSettings* ioSettings = FbxIOSettings::Create( fbxManager, IOSROOT );
	fbxManager->SetIOSettings( ioSettings );

	// load file
	FbxImporter* importer	  = FbxImporter::Create( fbxManager, "Importer" );
	bool		 importStatus = importer->Initialize( filename, -1, fbxManager->GetIOSettings() );
	FbxScene*	 fbxScene	  = nullptr;
	if ( importStatus )
	{
		fbxScene = FbxScene::Create( fbxManager, "FbxScene" );
		importer->Import( fbxScene );
		importer->Destroy();
	}
	else // error occurred
	{
		DebuggerPrintf( "Call to FbxImporter::Initialize() Failed.\n" );
		DebuggerPrintf( "Error returned: %s\n\n", importer->GetStatus().GetErrorString() );
		return;
	}

	//// change the axis system in fbx scene
	FbxAxisSystem defaultAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem sdAxisSystem		= FbxAxisSystem( FbxAxisSystem::EUpVector::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded );
	if ( defaultAxisSystem != sdAxisSystem )
	{
		sdAxisSystem.DeepConvertScene( fbxScene );
	}

	//// change the unit in the fbx scene
	FbxSystemUnit defaultUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	FbxSystemUnit sdUnit	  = FbxSystemUnit( SCALE_OF_THE_FB_SCENE );
	sdUnit.ConvertScene( fbxScene );

	// triangulate the scene
	FbxGeometryConverter fbxGeometryConverter( fbxManager );
	fbxGeometryConverter.Triangulate( fbxScene, true );

	// process the mesh and animation data and fill up vertex data
	FbxNode* rootNode	= fbxScene->GetRootNode();
	
	std::map<std::string, int> jointNameToJointIdMap;
	GetJointNameToJointIdMap( rootNode, jointNameToJointIdMap );

	int		 childCount = rootNode->GetChildCount();
	for ( int rootNodeChildIndex = 0; rootNodeChildIndex < childCount; rootNodeChildIndex++ )
	{
		FbxNode* immidiateChildNode = rootNode->GetChild( rootNodeChildIndex );
		bool	 isMeshNode			= immidiateChildNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh;
		if ( isMeshNode )
		{
			// cast this node to mesh node
			FbxMesh* meshNode = immidiateChildNode->GetMesh();

			std::map<int, std::vector<std::pair<int, float>>> controlPointIndexToJointIdMap;
			GetControlPointIndexToJointIdMap( meshNode, jointNameToJointIdMap, controlPointIndexToJointIdMap );

			// get all material indices of this mesh
			FbxLayerElementArrayTemplate<int>* materialIndices = nullptr;
			meshNode->GetMaterialIndices( &materialIndices );

			// calculate transform matrix of each mesh
			FbxAMatrix nodeTansformMatrix = CalculateFbxTransformMatrixOfFbxNode( immidiateChildNode );

			// for each triangle(polygon) in the mesh
			int triangleCount = meshNode->GetPolygonCount();
			int vertexCounter = 0;
			for ( int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++ )
			{
				// get the diffuse color for this triangle(polygon) from material
				Rgba8 diffuseColor = GetDiffuseColorForPolygon( materialIndices, triangleIndex, immidiateChildNode );

				// for each position in the triangle(polygon)
				int numPositionsInTriangle = meshNode->GetPolygonSize( triangleIndex );
				GUARANTEE_OR_DIE( numPositionsInTriangle == 3, "Polygon not a triangle in Fbx model" );
				for ( int positionInTriangle = 0; positionInTriangle < numPositionsInTriangle; positionInTriangle++ )
				{
					// get the index (this is equivalent to index in the index buffer)
					int controlPointIndex = meshNode->GetPolygonVertex( triangleIndex, positionInTriangle );

					// get the joint that affects this control point index
					std::vector<std::pair<int, float>> smoothSkinnedJointId = controlPointIndexToJointIdMap.at( controlPointIndex );
					outJointAndWeightsMapping.push_back( smoothSkinnedJointId );

					// get the vertex (this is vertex in vertex buffer or vertex array)
					Vec3 position = GetPositionOfControlPoint( meshNode, controlPointIndex, nodeTansformMatrix );

					// get normal
					Vec3 normal = GetNormalFromControlPointOrVertexCount( meshNode, controlPointIndex, vertexCounter );

					// finally create the 'SD Engine Vertex'
					Vertex_PCUTBN vertex;
					vertex.m_position = position;
					vertex.m_color	  = diffuseColor;
					vertex.m_normal	  = normal;
					outVerts.push_back( vertex );

					vertexCounter += 1;
				}
			}

			// TODO: remove later
			//break;
		}
	}

	// get the vertex to joint id mapping


	// destroy and release fbx memory before exiting
	fbxScene->Destroy();
	fbxManager->Destroy();
}


//----------------------------------------------------------------------------------------------------------
void GetJointNameToJointIdMap( FbxNode* fbxRootNode, std::map<std::string, int>& outRefMap )
{
	// find the root joint node
	int rootNodeChildCount = fbxRootNode->GetChildCount();
	FbxNode* rootJointNode	= nullptr;
	for (int rootNodeChildIndex = 0; rootNodeChildIndex < rootNodeChildCount; rootNodeChildIndex++)
	{
		FbxNode* childNode = fbxRootNode->GetChild( rootNodeChildIndex );
		bool	 isSkeletonNode = childNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton;
		if (isSkeletonNode)
		{
			rootJointNode = childNode;
			break;
		}
	}

	// process rest of skeleton 
	std::queue<FbxNode*> flattenedFbxNodeHierarchy;
	flattenedFbxNodeHierarchy.push( rootJointNode );
	int currentJointId = 0;
	while ( !flattenedFbxNodeHierarchy.empty() )
	{
		// add joint name - joint id pair in map
		FbxNode* node = flattenedFbxNodeHierarchy.front();
		outRefMap[ node->GetName() ] = currentJointId;

		// add joint's children
		int childCount				 = node->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; childIndex++)
		{
			FbxNode* childNode = node->GetChild( childIndex );
			flattenedFbxNodeHierarchy.push( childNode );
		}

		// remove processed joint 
		flattenedFbxNodeHierarchy.pop();
		currentJointId += 1;
	}
}


//----------------------------------------------------------------------------------------------------------
void GetControlPointIndexToJointIdMap( FbxMesh* meshNode, std::map<std::string, int> const& jointNameToJointIdMap, std::map<int, std::vector<std::pair<int, float>>>& outRefMap )
{
	int numberOfDeformers = meshNode->GetDeformerCount();
	for ( int deformerIndex = 0; deformerIndex < numberOfDeformers; deformerIndex++ )
	{
		FbxDeformer* meshDeformer = meshNode->GetDeformer( deformerIndex, FbxDeformer::eSkin );
		if ( !meshDeformer )
			continue;

		// only using skin deformers
		FbxSkin* meshSkinDeformer = reinterpret_cast<FbxSkin*>( meshDeformer );
		int		 numberOfClusters = meshSkinDeformer->GetClusterCount();
		for ( int clusterIndex = 0; clusterIndex < numberOfClusters; clusterIndex++ )
		{
			FbxCluster* cluster								= meshSkinDeformer->GetCluster( clusterIndex );
			FbxNode*	jointLinkNode						= cluster->GetLink();
			std::string jointName							= jointLinkNode->GetName();
			int			jointId								= jointNameToJointIdMap.at( jointName );

			int			numControlPointsAffectedByThisJoint = cluster->GetControlPointIndicesCount();
			int*		controlpointIndexesList				= cluster->GetControlPointIndices();
			double*		controlPointWeightList				= cluster->GetControlPointWeights();
			for ( int indexCount = 0; indexCount < numControlPointsAffectedByThisJoint; indexCount++ )
			{
				int	   controlPointIndex	   = controlpointIndexesList[ indexCount ];
				double controlPointWeight	   = controlPointWeightList[ indexCount ];

				if ( outRefMap.find( controlPointIndex ) == outRefMap.end() )
				{
					outRefMap[ controlPointIndex ] = { { jointId, static_cast<float>( controlPointWeight ) } };
				}
				else
				{
					auto iter = outRefMap.find( controlPointIndex );
					iter->second.push_back( { jointId, static_cast<float>( controlPointWeight ) } );
				}
			}
		}
	}
}