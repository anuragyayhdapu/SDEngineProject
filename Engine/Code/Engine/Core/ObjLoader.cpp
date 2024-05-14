#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ObjLoader.hpp"
#include "Engine/Core/MemoryFile.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"

#include <string>

#include <charconv>

//-------------------------------------------------------------------------
ObjLoader::ObjLoader( std::string const& fileName,
	std::vector<Vertex_PCUTBN>& vertexPCUTBNist, std::vector<unsigned int>& indexes,
	Mat44 const& transform )
{
	UNUSED( transform );

	double totalSecondsBeforeParsingFile = GetCurrentTimeSeconds();

	double startTime = GetCurrentTimeSeconds();

	// 1. load the entire file into memory
	MemoryFile* memoryFile = new MemoryFile( fileName.c_str() );
	uint8_t*	rawData	   = memoryFile->data();
	size_t		dataSize   = memoryFile->size();
	std::string stringData( reinterpret_cast<char*>( rawData ), dataSize );
	delete memoryFile;

	double endTime	 = GetCurrentTimeSeconds();
	double totaltime = ( endTime - startTime ) * 1000.0;
	DebuggerPrintf( "Memory File Reading Time: %f\n", totaltime );

	// 2. parse obj file and extract data

	startTime = GetCurrentTimeSeconds();

	StringsView const& stringList = SplitStringOnCarriageReturnAndNewLineStringView( stringData );

	endTime	  = GetCurrentTimeSeconds();
	totaltime = ( endTime - startTime ) * 1000.0;
	DebuggerPrintf( "\n Splitting String Time: %f\n", totaltime );

	std::vector<Vec3> unorderedVertexPositionList;
	std::vector<Vec3> unorderedNormalList;
	std::vector<Vec2> unorderedTextureUVList;

	unorderedVertexPositionList.reserve( stringList.size() );
	unorderedNormalList.reserve( stringList.size() );
	unorderedTextureUVList.reserve( stringList.size() );

	struct ObjFace
	{
		size_t m_numObjVertices = 0;

		struct ObjVertex
		{
			int m_vertexIndex	 = -1;
			int m_textureUVIndex = -1;
			int m_normalIndex	 = -1;
		};

		std::vector<ObjVertex> m_objVertices;
	};
	std::vector<ObjFace> objFaceList;

	objFaceList.reserve( stringList.size() );

	StringsView splitStringView;
	StringsView splitStringView2;

	for ( int index = 0; index < stringList.size(); index++ )
	{
		std::string_view line = stringList[ index ];

		char firstChar = line[ 0 ];
		switch ( firstChar )
		{
		case 'v': {
			char secondChar = line[ 1 ];

			// position
			if ( secondChar == ' ' )
			{
				SplitStringViewOnSpaces( line, splitStringView );
				StringsView const& vertexData = splitStringView;

				if ( !vertexData.empty() )
				{
					std::string_view const& xStr = vertexData[ 1 ];
					std::string_view const& yStr = vertexData[ 2 ];
					std::string_view const& zStr = vertexData[ 3 ];

					Vec3 vertex;
					std::from_chars( xStr.data(), xStr.data() + xStr.size(), vertex.x );
					std::from_chars( yStr.data(), yStr.data() + yStr.size(), vertex.y );
					std::from_chars( zStr.data(), zStr.data() + zStr.size(), vertex.z );

					unorderedVertexPositionList.emplace_back( vertex );
				}
			}
			// normal
			else if ( secondChar == 'n' )
			{
				SplitStringViewOnSpaces( line, splitStringView );
				StringsView const& normalData = splitStringView;
				if ( !normalData.empty() )
				{
					std::string_view const& xStr = normalData[ 1 ];
					std::string_view const& yStr = normalData[ 2 ];
					std::string_view const& zStr = normalData[ 3 ];

					Vec3 normal;
					std::from_chars( xStr.data(), xStr.data() + xStr.size(), normal.x );
					std::from_chars( yStr.data(), yStr.data() + yStr.size(), normal.y );
					std::from_chars( zStr.data(), zStr.data() + zStr.size(), normal.z );

					unorderedNormalList.emplace_back( normal );
				}
			}
			// texture
			else if ( secondChar == 't' )
			{
				SplitStringViewOnSpaces( line, splitStringView );
				StringsView const& textureData = splitStringView;
				if ( !textureData.empty() )
				{
					std::string_view const& xStr = textureData[ 1 ];
					std::string_view const& yStr = textureData[ 2 ];

					Vec2 textureUV;
					std::from_chars( xStr.data(), xStr.data() + xStr.size(), textureUV.x );
					std::from_chars( yStr.data(), yStr.data() + yStr.size(), textureUV.y );

					unorderedTextureUVList.emplace_back( textureUV );
				}
			}

			break;
		}
		// face
		case 'f': {

			SplitStringViewOnSpaces( line, splitStringView );
			StringsView const& faceData = splitStringView;
			if ( !faceData.empty() )
			{
				ObjFace objFace;
				objFace.m_numObjVertices = faceData.size() - 1;
				objFace.m_objVertices.reserve( objFace.m_numObjVertices );

				for ( int faceVertexIndex = 1; faceVertexIndex < faceData.size(); faceVertexIndex++ )
				{
					objFace.m_objVertices.emplace_back();
					ObjFace::ObjVertex& objVertex = objFace.m_objVertices.back();

					std::string_view const& vertexStr	   = faceData[ faceVertexIndex ];
					SplitStringsViewOnDelimiter( vertexStr, '/', splitStringView2 );
					StringsView const& vertexIndecies = splitStringView2;

					// vertex position index
					std::string_view const& vertexPositionIndexStr = vertexIndecies[ 0 ];

					std::from_chars( vertexPositionIndexStr.data(), vertexPositionIndexStr.data() + vertexPositionIndexStr.size(), objVertex.m_vertexIndex );

					// vertex texture uv index
					if ( vertexIndecies.size() > 1 )
					{
						std::string_view const& vertexTextureUVIndexStr = vertexIndecies[ 1 ];
						if ( !vertexTextureUVIndexStr.empty() )
						{
							std::from_chars( vertexTextureUVIndexStr.data(), vertexTextureUVIndexStr.data() + vertexTextureUVIndexStr.size(), objVertex.m_textureUVIndex );
						}
					}

					// vertex normal index
					if ( vertexIndecies.size() > 2 )
					{
						std::string_view const& vertexNormalIndexStr = vertexIndecies[ 2 ];
						if ( !vertexNormalIndexStr.empty() )
						{
							std::from_chars( vertexNormalIndexStr.data(), vertexNormalIndexStr.data() + vertexNormalIndexStr.size(), objVertex.m_normalIndex );
						}
					}

					//objFace.m_objVertices.emplace_back( objVertex );
				}

				objFaceList.emplace_back( objFace );
			}

			break;
		}

		default:
			break;
		}
	}


	double totalSecondsAfterParsingFile		  = GetCurrentTimeSeconds();
	double totalSecondsBeforeCreatingVertexes = GetCurrentTimeSeconds();


	// 3. calculate vertex data
	// 3.1 vertex position data
	std::vector<Vec3> orderedVertexPositionList;
	std::vector<Vec3> orderedNormalList;
	std::vector<Vec2> orderedTextureUVList;
	bool			  isNormalDataPresentInFile	   = !unorderedNormalList.empty();
	bool			  isFaceDataPresentInFile	   = !objFaceList.empty();
	bool			  isTextureUVDataPresentInFile = !unorderedTextureUVList.empty();

	if ( isFaceDataPresentInFile )
	{
		orderedVertexPositionList.reserve( unorderedVertexPositionList.size() );
		if ( isNormalDataPresentInFile )
		{
			orderedNormalList.reserve( unorderedVertexPositionList.size() );
		}
		if ( isTextureUVDataPresentInFile )
		{
			orderedTextureUVList.reserve( unorderedVertexPositionList.size() );
		}

		for ( int index = 0; index < objFaceList.size(); index++ )
		{
			ObjFace const& objFace = objFaceList[ index ];

			int	   iterator	   = 1;
			size_t numVertices = objFace.m_numObjVertices;
			while ( iterator < numVertices - 1 )
			{
				// vertex A
				ObjFace::ObjVertex const& objVertexA	  = objFace.m_objVertices[ 0 ];
				int						  vertexIndexA	  = objVertexA.m_vertexIndex - 1;
				Vec3					  vertexPositionA = unorderedVertexPositionList[ vertexIndexA ];
				orderedVertexPositionList.emplace_back( vertexPositionA );

				// vertex B
				ObjFace::ObjVertex const& objVertexB	  = objFace.m_objVertices[ iterator ];
				int						  vertexIndexB	  = objVertexB.m_vertexIndex - 1;
				Vec3					  vertexPositionB = unorderedVertexPositionList[ vertexIndexB ];
				orderedVertexPositionList.emplace_back( vertexPositionB );

				// vertex C
				ObjFace::ObjVertex const& objVertexC	  = objFace.m_objVertices[ iterator + 1 ];
				int						  vertexIndexC	  = objVertexC.m_vertexIndex - 1;
				Vec3					  vertexPositionC = unorderedVertexPositionList[ vertexIndexC ];
				orderedVertexPositionList.emplace_back( vertexPositionC );


				if ( isNormalDataPresentInFile )
				{
					int	 normalIndexA = objVertexA.m_normalIndex - 1;
					Vec3 normalA	  = unorderedNormalList[ normalIndexA ];
					orderedNormalList.emplace_back( normalA );

					int	 normalIndexB = objVertexB.m_normalIndex - 1;
					Vec3 normalB	  = unorderedNormalList[ normalIndexB ];
					orderedNormalList.emplace_back( normalB );

					int	 normalIndexC = objVertexC.m_normalIndex - 1;
					Vec3 normalC	  = unorderedNormalList[ normalIndexC ];
					orderedNormalList.emplace_back( normalC );
				}

				if ( isTextureUVDataPresentInFile )
				{
					int	 textureUVIndexA = objVertexA.m_textureUVIndex - 1;
					Vec2 textureUVA		 = unorderedTextureUVList[ textureUVIndexA ];
					orderedTextureUVList.emplace_back( textureUVA );

					int	 textureUVIndexB = objVertexB.m_textureUVIndex - 1;
					Vec2 textureUVB		 = unorderedTextureUVList[ textureUVIndexB ];
					orderedTextureUVList.emplace_back( textureUVB );

					int	 textureUVIndexC = objVertexC.m_textureUVIndex - 1;
					Vec2 textureUVC		 = unorderedTextureUVList[ textureUVIndexC ];
					orderedTextureUVList.emplace_back( textureUVC );
				}


				iterator++;
			}
		}
	}

	// missing data handling
	if ( !isFaceDataPresentInFile )
	{
		orderedVertexPositionList = unorderedVertexPositionList;
	}

	if ( !isTextureUVDataPresentInFile ) // default initialize texture uvs to (0,0)
	{
		size_t numVertexes	 = orderedVertexPositionList.size();
		orderedTextureUVList = std::vector<Vec2>( numVertexes, Vec2::ZERO );
	}

	if ( !isNormalDataPresentInFile ) // calculate normals from raw vertex position
	{
		orderedNormalList.reserve( unorderedVertexPositionList.size() );

		for ( int index = 0; index < orderedVertexPositionList.size(); index += 3 )
		{
			// vertices of triangle A,B,C
			Vec3 A = orderedVertexPositionList[ index ];
			Vec3 B = orderedVertexPositionList[ index + 1 ];
			Vec3 C = orderedVertexPositionList[ index + 2 ];

			Vec3 sideAB = B - A;
			Vec3 sideBC = C - B;

			Vec3 faceNormal = CrossProduct3D( sideAB, sideBC );
			faceNormal.Normalize();

			orderedNormalList.emplace_back( faceNormal );
			orderedNormalList.emplace_back( faceNormal );
			orderedNormalList.emplace_back( faceNormal );
		}
	}


	// 4. fill the Vertx_PNCU data
	vertexPCUTBNist.reserve( orderedVertexPositionList.size() );

	for ( int index = 0; index < orderedVertexPositionList.size(); index++ )
	{
		Vertex_PCUTBN vertex;
		vertex.m_position	 = orderedVertexPositionList[ index ];
		vertex.m_normal		 = orderedNormalList[ index ];
		vertex.m_uvTexCoords = orderedTextureUVList[ index ];
		vertex.m_color		 = Rgba8::WHITE;

		vertexPCUTBNist.emplace_back( vertex );
		indexes.emplace_back( index );
	}


	// 5. calculate statistics and print to dev console

	m_loadingStatistics.m_fileName = fileName;

	// time
	double totalSecondsAfterCreatingVertexes = GetCurrentTimeSeconds();
	double totalParsingTimeInSeconds		 = totalSecondsAfterParsingFile - totalSecondsBeforeParsingFile;
	m_loadingStatistics.m_time.m_parseInMs	 = totalParsingTimeInSeconds * 1000.0;
	double totalCreatingTimeInSeconds		 = totalSecondsAfterCreatingVertexes - totalSecondsBeforeCreatingVertexes;
	m_loadingStatistics.m_time.m_createInMs	 = totalCreatingTimeInSeconds * 1000.0;

	// memory
	m_loadingStatistics.m_fileData.m_vertexs	   = unorderedVertexPositionList.size();
	m_loadingStatistics.m_fileData.m_textureCoords = unorderedTextureUVList.size();
	m_loadingStatistics.m_fileData.m_normals	   = unorderedNormalList.size();
	m_loadingStatistics.m_fileData.m_faces		   = objFaceList.size();
	m_loadingStatistics.m_fileData.m_triangles	   = vertexPCUTBNist.size() / 3;
	m_loadingStatistics.m_loadedMesh.m_vertexes	   = vertexPCUTBNist.size();
	m_loadingStatistics.m_loadedMesh.m_indexes	   = indexes.size();

	// print to dev console
	Strings statisticsStrings = m_loadingStatistics.GetStatisticsString();
	for ( int index = 0; index < statisticsStrings.size(); index++ )
	{
		std::string const& statisticsString = statisticsStrings[ index ];
		g_theDevConsole->AddLine( Rgba8::WHITE, statisticsString );
		DebuggerPrintf( "%s\n", statisticsString.c_str() );
	}
}


std::vector<std::string> ObjLoader::LoadingStatistics::GetStatisticsString() const
{
	Strings statisticsStrings;

	std::string fileNameStatsString	  = Stringf( "Loaded .obj file %s", m_fileName.c_str() );
	std::string fileDataStatsString	  = Stringf( "  [file data]   vertexes: %i  texture coordinates: %i  normals: %i  faces: %i  triangles: %i",
		  m_fileData.m_vertexs, m_fileData.m_textureCoords, m_fileData.m_normals, m_fileData.m_faces,
		  m_fileData.m_triangles );
	std::string loadedMeshStatsString = Stringf( "  [loaded mesh] vertexes: %i  indexes: %i",
		m_loadedMesh.m_vertexes, m_loadedMesh.m_indexes );
	std::string timeStatsString		  = Stringf( "  [time]        parse: %f ms  create: %f ms",
			  m_time.m_parseInMs, m_time.m_createInMs );

	statisticsStrings.emplace_back( "" );
	statisticsStrings.emplace_back( "--------------------------------------------------------------------------------------------------" );
	statisticsStrings.emplace_back( fileNameStatsString );
	statisticsStrings.emplace_back( fileDataStatsString );
	statisticsStrings.emplace_back( loadedMeshStatsString );
	statisticsStrings.emplace_back( timeStatsString );
	statisticsStrings.emplace_back( "--------------------------------------------------------------------------------------------------" );
	statisticsStrings.emplace_back( "" );

	return statisticsStrings;
}
