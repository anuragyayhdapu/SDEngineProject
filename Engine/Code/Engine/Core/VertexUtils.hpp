#pragma once

#include <vector>

#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/ConvexPolly2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/FloatRange.hpp"


class Vertex_PCU;
class Vertex_PCUTBN;

//----------------------------------------------------------------------------------------------------------
// 2d transformation
void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScale, float rotationDegreeAboutZ, Vec2 const& translationXY);


//----------------------------------------------------------------------------------------------------------
// 2d vertex addition
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color);
void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color);
void AddVertsForAABB2(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 uvAtMins = Vec2::ZERO, Vec2 uvAtMaxs = Vec2::ONE);
void AddVertsForAABB2(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float z, Rgba8 const& color, Vec2 uvAtMins = Vec2::ZERO, Vec2 uvAtMaxs = Vec2::ONE);
void AddVertsForOBB2D(std::vector<Vertex_PCU>& verts, OBB2 const& orientedBox, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& startPoint, Vec2 const& endpoint, float thickness, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& startPoint, Vec2 const& endpoint, float z, float thickness, Rgba8 const& color);
void AddVertsForRing2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float ringThickness, Rgba8 const& color);
void AddVertsForSector2D(std::vector<Vertex_PCU>& vets, Vec2 const& center, float radius, float startOrientationDegrees, float endOrientationDegrees, float thickness, Rgba8 const& color);
void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color = Rgba8::WHITE);


//----------------------------------------------------------------------------------------------------------
// 2d indexed vertex addition
void AddVertsForHexagonBoundry2DIndexed( std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec2 const& hexagonCenter, float circumradius, float boundryThickness, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForHexagon2DIndexed( std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec2 const& hexagonCenter, float circumradius, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForConvexPolly2DIndexed( std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, ConvexPolly2 const& convexPolly2, Rgba8 const& color = Rgba8::WHITE );
void AddVertsForLineSegment2DIndexed( std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec2 const& startPoint, Vec2 const& endpoint, float thickness, Rgba8 const& color );


//----------------------------------------------------------------------------------------------------------
// 2d wireframe
void AddVertsForWireframeAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float frameThickness = 0.1f, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForWireframeAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float z, float frameThickness = 0.1f, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForWireframeConvexPolly2DIndexed( std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, ConvexPolly2 const& convexPolly2, float frameThickness = 0.1f, Rgba8 const& color = Rgba8::WHITE );


//----------------------------------------------------------------------------------------------------------
// 3d transformation
void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, Mat44 const& transform);


//----------------------------------------------------------------------------------------------------------
// 3d vertex addition
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color, Vec2 uvAtMins = Vec2::ZERO, Vec2 uvAtMaxs = Vec2::ONE);
void AddVertsForCylinderZ3D(std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, int numSlices, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE);
void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE, int numSlices = 16);
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE, int numSlices = 16);
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE, int numLatitudeSlices = 16);
void AddVertsForArrow3D( std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE, int numSlices = 16 );
void AddVertsForPlane3D( std::vector<Vertex_PCU>& verts, Plane3 const& plane3D, float width = 10.f, float height = 10.f, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE );
void AddVertsForOBB3D( std::vector<Vertex_PCU>& verts, OBB3 const& obb3D, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE );
void AddVertsForSectorRingWithArrow3D( std::vector<Vertex_PCU>& verts, Vec3 const& planeNormal, Vec3 const& sectorCenter, float sectorRadius, float sectorStartAngle, float sectorEndAngle, float ringThickness, float arrowRadius, int numSlices = 16, Rgba8 color = Rgba8::WHITE);

//----------------------------------------------------------------------------------------------------------
// 3d indexed vertex addition
void AddVertsForQuad3D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE);


//----------------------------------------------------------------------------------------------------------
// 3d indexed vertex addition with Vertex_PCUTBN
void AddVertsForQuad3DPCUTBN(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3DPCUTBN(std::vector<Vertex_PCUTBN>& vertexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForRoundedQuad3DPCUTBN(std::vector<Vertex_PCUTBN>& vertexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForAABB3DPCUTBN(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, AABB3 const& bounds, Rgba8 const& color, AABB2 uvs);
void AddVertsForSphere3DPCUTBN(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvs = AABB2::ZERO_TO_ONE, int numLatitudeSlices = 16);


//----------------------------------------------------------------------------------------------------------
// 3d wireframe
void AddVertsForWireframeAABB3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, float frameThickness = 0.1f, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForWireframeQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, float frameThickness = 0.1f, Rgba8 const& color = Rgba8::WHITE);


//----------------------------------------------------------------------------------------------------------
// 3d line primitive
void AddVertsForLineUsingLineList3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, Rgba8 const& color = Rgba8::WHITE);


//----------------------------------------------------------------------------------------------------------
// misc
AABB2 GetVertexBounds2D( std::vector<Vertex_PCU> const& verts );
void CalculateTangetSpaceBasisVectorForVertex_PCUTBN( std::vector<Vertex_PCUTBN>& outCpuVerts, std::vector<unsigned int> const& indexes );