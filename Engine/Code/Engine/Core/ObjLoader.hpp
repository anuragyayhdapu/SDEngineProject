#pragma once


#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"

#include <string>
#include <vector>


//-------------------------------------------------------------------------
class ObjLoader
{
public:
	ObjLoader( std::string const& fileName,
		std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes,
		Mat44 const& transform );

	ObjLoader() = delete;

protected:
	struct LoadingStatistics
	{
		std::string m_fileName = "UNKOWN FILE NAME";

		struct FileData
		{
			size_t m_vertexs	   = 0;
			size_t m_textureCoords = 0;
			size_t m_normals	   = 0;
			size_t m_faces		   = 0;
			size_t m_triangles	   = 0;
		} m_fileData;

		struct LoadedMesh
		{
			size_t m_vertexes = 0;
			size_t m_indexes  = 0;
		} m_loadedMesh;

		struct Time
		{
			double m_parseInMs	= 0.f;
			double m_createInMs = 0.f;
		} m_time;

		std::vector<std::string> GetStatisticsString() const;

	} m_loadingStatistics;
};