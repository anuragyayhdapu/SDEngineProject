#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/EngineCommon.hpp"


void TransformVertexArrayXY3D( int numVerts, Vertex_PCU* verts, float uniformScale,
	float rotationDegreeAboutZ, Vec2 const& translationXY )
{
	for ( int index = 0; index < numVerts; ++index )
	{
		Vec3& vertex = verts[ index ].m_position;
		TransformPositionXY3D( vertex, uniformScale, rotationDegreeAboutZ, translationXY );
	}
}


//----------------------------------------------------------------------------------------

void AddVertsForCapsule2D( std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color )
{
	Vec2 bone	 = boneEnd - boneStart;
	Vec2 topLeft = bone.GetRotated90Degrees();
	topLeft.SetLength( radius );

	Vec2 TL = boneStart + topLeft;
	Vec2 BL = boneStart - topLeft;
	Vec2 TR = TL + bone;
	Vec2 BR = BL + bone;

	// bone box
	// 1st triangle
	verts.push_back( Vertex_PCU( Vec3( BL ), color ) );
	verts.push_back( Vertex_PCU( Vec3( TR ), color ) );
	verts.push_back( Vertex_PCU( Vec3( TL ), color ) );

	// 2nd triangle
	verts.push_back( Vertex_PCU( Vec3( BL ), color ) );
	verts.push_back( Vertex_PCU( Vec3( BR ), color ) );
	verts.push_back( Vertex_PCU( Vec3( TR ), color ) );

	// end-hemisphers
	constexpr int TOTAL_HEMISPHERE_TRIANGLES = 64;
	float		  deltaThetaDegrees			 = 180.f / ( float ) ( TOTAL_HEMISPHERE_TRIANGLES );

	// left hemisphere
	Vec2 radiusVector = bone.GetRotated90Degrees();
	radiusVector.SetLength( radius );
	for ( int triangleIndex = 0; triangleIndex < TOTAL_HEMISPHERE_TRIANGLES; triangleIndex++ )
	{
		Vec2 firstpoint = boneStart + radiusVector;

		radiusVector.RotateDegrees( deltaThetaDegrees );
		Vec2 secondpoint = boneStart + radiusVector;

		Vec2 thirdpoint = boneStart;

		verts.push_back( Vertex_PCU( Vec3( firstpoint ), color ) );
		verts.push_back( Vertex_PCU( Vec3( secondpoint ), color ) );
		verts.push_back( Vertex_PCU( Vec3( thirdpoint ), color ) );
	}

	// right hemisphere
	radiusVector = -bone.GetRotated90Degrees();
	radiusVector.SetLength( radius );
	for ( int triangleIndex = 0; triangleIndex < TOTAL_HEMISPHERE_TRIANGLES; triangleIndex++ )
	{
		Vec2 firstpoint = boneEnd + radiusVector;

		radiusVector.RotateDegrees( deltaThetaDegrees );
		Vec2 secondpoint = boneEnd + radiusVector;

		Vec2 thirdpoint = boneEnd;

		verts.push_back( Vertex_PCU( Vec3( firstpoint ), color ) );
		verts.push_back( Vertex_PCU( Vec3( secondpoint ), color ) );
		verts.push_back( Vertex_PCU( Vec3( thirdpoint ), color ) );
	}
}

void AddVertsForDisc2D( std::vector<Vertex_PCU>& allVerts, Vec2 const& center, float radius, Rgba8 const& color )
{
	constexpr int TOTAL_DISC_TRIANGLES = 128;
	constexpr int TOTAL_DISC_VERTEXAS  = TOTAL_DISC_TRIANGLES * 3;

	std::vector<Vertex_PCU> verts;
	verts.resize( TOTAL_DISC_VERTEXAS );

	float deltaThetaDegrees = 360.f / ( float ) ( TOTAL_DISC_TRIANGLES );

	float thetaDegrees = 0.f;
	int	  vertexIndex  = 0;

	// generating first three points
	verts[ 0 ].m_position = Vec3();
	verts[ 1 ].m_position = Vec3( Vec2::MakeFromPolarDegrees( thetaDegrees, radius ) );
	thetaDegrees += deltaThetaDegrees;
	verts[ 2 ].m_position = Vec3( Vec2::MakeFromPolarDegrees( thetaDegrees, radius ) );

	vertexIndex = 3;
	for ( ; vertexIndex < TOTAL_DISC_VERTEXAS - 3; vertexIndex++ )
	{
		verts[ vertexIndex ].m_position = verts[ 0 ].m_position;
		vertexIndex++;

		verts[ vertexIndex ].m_position = verts[ vertexIndex - 2 ].m_position;
		vertexIndex++;
		thetaDegrees += deltaThetaDegrees;

		verts[ vertexIndex ].m_position = Vec3( Vec2::MakeFromPolarDegrees( thetaDegrees, radius ) );
	}

	// last three points
	verts[ vertexIndex ].m_position = verts[ 0 ].m_position;
	vertexIndex++;

	verts[ vertexIndex ].m_position = verts[ vertexIndex - 2 ].m_position;
	vertexIndex++;
	thetaDegrees += deltaThetaDegrees;

	verts[ vertexIndex ].m_position = verts[ 1 ].m_position;

	// translate & color all vertexas
	for ( int index = 0; index < verts.size(); index++ )
	{
		verts[ index ].m_color = color;
		verts[ index ].m_position += center;
	}

	allVerts.insert( allVerts.end(), verts.begin(), verts.end() );
}


void AddVertsForAABB2( std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs )
{
	// calculate corner points
	Vec2 bottomLeft( bounds.m_mins );
	Vec2 bottomRight( bounds.m_maxs.x, bounds.m_mins.y );
	Vec2 topRight( bounds.m_maxs );
	Vec2 topLeft( bounds.m_mins.x, bounds.m_maxs.y );

	const Vec2 uvBottomLeft( uvAtMins );
	const Vec2 uvBottomRight( uvAtMaxs.x, uvAtMins.y );
	const Vec2 uvTopRight( uvAtMaxs );
	const Vec2 uvTopLeft( uvAtMins.x, uvAtMaxs.y );

	// first triangle
	verts.push_back( Vertex_PCU( Vec3( bottomLeft ), color, uvBottomLeft ) );
	verts.push_back( Vertex_PCU( Vec3( topRight ), color, uvTopRight ) );
	verts.push_back( Vertex_PCU( Vec3( topLeft ), color, uvTopLeft ) );

	// second triangle
	verts.push_back( Vertex_PCU( Vec3( bottomLeft ), color, uvBottomLeft ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight ), color, uvBottomRight ) );
	verts.push_back( Vertex_PCU( Vec3( topRight ), color, uvTopRight ) );
}

void AddVertsForAABB2( std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float z, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs )
{
	// calculate corner points
	Vec2 bottomLeft( bounds.m_mins );
	Vec2 bottomRight( bounds.m_maxs.x, bounds.m_mins.y );
	Vec2 topRight( bounds.m_maxs );
	Vec2 topLeft( bounds.m_mins.x, bounds.m_maxs.y );

	const Vec2 uvBottomLeft( uvAtMins );
	const Vec2 uvBottomRight( uvAtMaxs.x, uvAtMins.y );
	const Vec2 uvTopRight( uvAtMaxs );
	const Vec2 uvTopLeft( uvAtMins.x, uvAtMaxs.y );

	// first triangle
	verts.push_back( Vertex_PCU( Vec3( bottomLeft.x, bottomLeft.y, z ), color, uvBottomLeft ) );
	verts.push_back( Vertex_PCU( Vec3( topRight.x, topRight.y, z ), color, uvTopRight ) );
	verts.push_back( Vertex_PCU( Vec3( topLeft.x, topLeft.y, z ), color, uvTopLeft ) );

	// second triangle
	verts.push_back( Vertex_PCU( Vec3( bottomLeft.x, bottomLeft.y, z ), color, uvBottomLeft ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight.x, bottomRight.y, z ), color, uvBottomRight ) );
	verts.push_back( Vertex_PCU( Vec3( topRight.x, topRight.y, z ), color, uvTopRight ) );
}

void AddVertsForOBB2D( std::vector<Vertex_PCU>& verts, OBB2 const& orientedBox, Rgba8 const& color )
{
	// obb2 data
	Vec2 const& i		   = orientedBox.m_iBasisNormal;
	float		halfWidth  = orientedBox.m_halfDimensions.x;
	float		halfHeight = orientedBox.m_halfDimensions.y;
	Vec2 const& center	   = orientedBox.m_center;

	// get j basis normal vector, right vector & up vector
	Vec2 j	   = i.GetRotated90Degrees();
	Vec2 right = i * halfWidth;
	Vec2 up	   = j * halfHeight;

	// get corner points
	Vec2 topRight	 = center + right + up;
	Vec2 topLeft	 = center - right + up;
	Vec2 bottomLeft	 = center - right - up;
	Vec2 bottomRight = center + right - up;

	// first triangle
	verts.push_back( Vertex_PCU( Vec3( bottomLeft ), color ) );
	verts.push_back( Vertex_PCU( Vec3( topRight ), color ) );
	verts.push_back( Vertex_PCU( Vec3( topLeft ), color ) );

	// second triangle
	verts.push_back( Vertex_PCU( Vec3( bottomLeft ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight ), color ) );
	verts.push_back( Vertex_PCU( Vec3( topRight ), color ) );
}

void AddVertsForLineSegment2D( std::vector<Vertex_PCU>& verts, Vec2 const& startPoint, Vec2 const& endpoint, float thickness, Rgba8 const& color )
{
	float halfThickness = thickness * 0.5f;
	float ninteyDegrees = 90.0f;
	float deltaDegrees	= 45.0f;

	float orientationDegrees = ( Vec2( endpoint ) - Vec2( startPoint ) ).GetOrientationDegrees();
	float R					 = GetDistance2D( startPoint, endpoint );

	float xTopLeft = R * CosDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );
	float yTopLeft = R * SinDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );

	float xBottomLeft = R * CosDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );
	float yBottomLeft = R * SinDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );

	float xTopRight = R * CosDegrees( orientationDegrees + deltaDegrees );
	float yTopRight = R * SinDegrees( orientationDegrees + deltaDegrees );

	float xBottomRight = R * CosDegrees( orientationDegrees - deltaDegrees );
	float yBottomRight = R * SinDegrees( orientationDegrees - deltaDegrees );

	Vec2 topLeft( xTopLeft, yTopLeft );
	Vec2 topRight( xTopRight, yTopRight );
	Vec2 bottomRight( xBottomRight, yBottomRight );
	Vec2 bottomLeft( xBottomLeft, yBottomLeft );

	topLeft.SetLength( halfThickness );
	topRight.SetLength( halfThickness );
	bottomRight.SetLength( halfThickness );
	bottomLeft.SetLength( halfThickness );

	topLeft += startPoint;
	topRight += endpoint;
	bottomRight += endpoint;
	bottomLeft += startPoint;

	verts.push_back( Vertex_PCU( Vec3( topLeft ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight ), color ) );
	verts.push_back( Vertex_PCU( Vec3( topRight ), color ) );

	verts.push_back( Vertex_PCU( Vec3( topLeft ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomLeft ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight ), color ) );
}

void AddVertsForLineSegment2D( std::vector<Vertex_PCU>& verts, Vec2 const& startPoint, Vec2 const& endpoint, float z, float thickness, Rgba8 const& color )
{
	float halfThickness = thickness * 0.5f;
	float ninteyDegrees = 90.0f;
	float deltaDegrees	= 45.0f;

	float orientationDegrees = ( Vec2( endpoint ) - Vec2( startPoint ) ).GetOrientationDegrees();
	float R					 = GetDistance2D( startPoint, endpoint );

	float xTopLeft = R * CosDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );
	float yTopLeft = R * SinDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );

	float xBottomLeft = R * CosDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );
	float yBottomLeft = R * SinDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );

	float xTopRight = R * CosDegrees( orientationDegrees + deltaDegrees );
	float yTopRight = R * SinDegrees( orientationDegrees + deltaDegrees );

	float xBottomRight = R * CosDegrees( orientationDegrees - deltaDegrees );
	float yBottomRight = R * SinDegrees( orientationDegrees - deltaDegrees );

	Vec2 topLeft( xTopLeft, yTopLeft );
	Vec2 topRight( xTopRight, yTopRight );
	Vec2 bottomRight( xBottomRight, yBottomRight );
	Vec2 bottomLeft( xBottomLeft, yBottomLeft );

	topLeft.SetLength( halfThickness );
	topRight.SetLength( halfThickness );
	bottomRight.SetLength( halfThickness );
	bottomLeft.SetLength( halfThickness );

	topLeft += startPoint;
	topRight += endpoint;
	bottomRight += endpoint;
	bottomLeft += startPoint;

	verts.push_back( Vertex_PCU( Vec3( topLeft, z ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight, z ), color ) );
	verts.push_back( Vertex_PCU( Vec3( topRight, z ), color ) );

	verts.push_back( Vertex_PCU( Vec3( topLeft, z ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomLeft, z ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight, z ), color ) );
}

void AddVertsForRing2D( std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float ringThickness, Rgba8 const& color )
{
	constexpr int TOTAL_RING_TRIANGLES = 128;
	float		  deltaThetaDegrees	   = 360.f / ( float ) ( TOTAL_RING_TRIANGLES );

	Vec2 radiusVector( radius, 0.f );
	for ( int triangleIndex = 0; triangleIndex < TOTAL_RING_TRIANGLES; triangleIndex++ )
	{
		Vec2 firstpoint = center + radiusVector;

		radiusVector.RotateDegrees( deltaThetaDegrees );
		Vec2 secondpoint = center + radiusVector;

		AddVertsForLineSegment2D( verts, firstpoint, secondpoint, ringThickness, color );
	}
}

void AddVertsForSector2D( std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius,
	float startOrientationDegrees, float endOrientationDegrees, float thickness, Rgba8 const& color )
{
	constexpr int NUM_TRIANGLES_IN_180_DEGREES_SECTOR = 64;
	float		  sectorRange						  = fabsf( endOrientationDegrees - startOrientationDegrees );
	const int	  numTriangles						  = ( int ) ( ( NUM_TRIANGLES_IN_180_DEGREES_SECTOR / 180.f ) * sectorRange );

	float deltaThetaDegrees = sectorRange / ( float ) ( numTriangles );

	// sector arc
	Vec2 radiusVector = Vec2::MakeFromPolarDegrees( startOrientationDegrees, radius );
	for ( int triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++ )
	{
		Vec2 firstpoint = center + radiusVector;

		radiusVector.RotateDegrees( deltaThetaDegrees );
		Vec2 secondpoint = center + radiusVector;

		AddVertsForLineSegment2D( verts, firstpoint, secondpoint, thickness, color );
	}

	// sector start and end line
	Vec2 startLinePoint = Vec2::MakeFromPolarDegrees( startOrientationDegrees, radius );
	AddVertsForLineSegment2D( verts, center, center + startLinePoint, thickness, color );
	Vec2 endLinePoint = Vec2::MakeFromPolarDegrees( endOrientationDegrees, radius );
	AddVertsForLineSegment2D( verts, center, center + endLinePoint, thickness, color );
}

void AddVertsForArrow2D( std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color )
{
	AddVertsForLineSegment2D( verts, tailPos, tipPos, lineThickness, color );

	// arrow tip
	const float ARROW_TIP_ANGLE = 45.f;

	Vec2 lineTipToTail = tailPos - tipPos;
	Vec2 bottomTip	   = lineTipToTail.GetRotatedDegrees( ARROW_TIP_ANGLE );
	bottomTip.SetLength( arrowSize );
	AddVertsForLineSegment2D( verts, tipPos, bottomTip + tipPos, lineThickness, color );

	Vec2 topTip = bottomTip.GetRotatedMinus90Degrees();
	AddVertsForLineSegment2D( verts, tipPos, topTip + tipPos, lineThickness, color );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2DIndexed( std::vector<Vertex_PCUTBN>& verts, std::vector<int>& indexes, Vec2 const& startPoint, Vec2 const& endpoint, float thickness, Rgba8 const& color )
{
	float halfThickness = thickness * 0.5f;
	float ninteyDegrees = 90.0f;
	float deltaDegrees	= 45.0f;

	float orientationDegrees = ( Vec2( endpoint ) - Vec2( startPoint ) ).GetOrientationDegrees();
	float R					 = GetDistance2D( startPoint, endpoint );

	float xTopLeft = R * CosDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );
	float yTopLeft = R * SinDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );

	float xBottomLeft = R * CosDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );
	float yBottomLeft = R * SinDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );

	float xTopRight = R * CosDegrees( orientationDegrees + deltaDegrees );
	float yTopRight = R * SinDegrees( orientationDegrees + deltaDegrees );

	float xBottomRight = R * CosDegrees( orientationDegrees - deltaDegrees );
	float yBottomRight = R * SinDegrees( orientationDegrees - deltaDegrees );

	Vec2 topLeft( xTopLeft, yTopLeft );
	Vec2 topRight( xTopRight, yTopRight );
	Vec2 bottomRight( xBottomRight, yBottomRight );
	Vec2 bottomLeft( xBottomLeft, yBottomLeft );

	topLeft.SetLength( halfThickness );
	topRight.SetLength( halfThickness );
	bottomRight.SetLength( halfThickness );
	bottomLeft.SetLength( halfThickness );

	topLeft += startPoint;
	topRight += endpoint;
	bottomRight += endpoint;
	bottomLeft += startPoint;

	int vertsLastIndexBeforePushing = ( int ) verts.size();

	verts.push_back( Vertex_PCUTBN( Vec3( bottomLeft ), Vec3( 0.f, 0.f, 1.f ), color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bottomRight ), Vec3( 0.f, 0.f, 1.f ), color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( topRight ), Vec3( 0.f, 0.f, 1.f ), color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( topLeft ), Vec3( 0.f, 0.f, 1.f ), color ) );

	int				 l			= vertsLastIndexBeforePushing;
	std::vector<int> newIndexes = { l + 0, l + 1, l + 2, l + 0, l + 2, l + 3 };
	indexes.insert( indexes.end(), newIndexes.begin(), newIndexes.end() );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForHexagonBoundry2DIndexed( std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec2 const& hexagonCenter, float circumradius, float boundryThickness, Rgba8 const& color )
{
	float const STARTING_ANGLE			 = 0.f;
	float const HEX_TRIANGLE_ARC_DEGREES = 60.f;

	Vec2 hexCorner0 = Vec2::MakeFromPolarDegrees( 0.f, circumradius );
	Vec2 hexCorner1 = hexCorner0.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner2 = hexCorner1.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner3 = hexCorner2.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner4 = hexCorner3.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner5 = hexCorner4.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );

	float halfThickness = boundryThickness * 0.5f;

	// hexCorner0
	Vec2 outerPoint1 = hexCorner0;
	outerPoint1.SetLength( halfThickness );
	Vec2 innerPoint0 = -1.f * outerPoint1;
	outerPoint1 += hexCorner0;
	innerPoint0 += hexCorner0;

	// hexCorner1
	Vec2 outerPoint3 = hexCorner1;
	outerPoint3.SetLength( halfThickness );
	Vec2 innerPoint2 = -1.f * outerPoint3;
	outerPoint3 += hexCorner1;
	innerPoint2 += hexCorner1;

	// hexCorner2
	Vec2 outerPoint5 = hexCorner2;
	outerPoint5.SetLength( halfThickness );
	Vec2 innerPoint4 = -1.f * outerPoint5;
	outerPoint5 += hexCorner2;
	innerPoint4 += hexCorner2;

	// hexCorner3
	Vec2 outerPoint7 = hexCorner3;
	outerPoint7.SetLength( halfThickness );
	Vec2 innerPoint6 = -1.f * outerPoint7;
	outerPoint7 += hexCorner3;
	innerPoint6 += hexCorner3;

	// hexCorner4
	Vec2 outerPoint9 = hexCorner4;
	outerPoint9.SetLength( halfThickness );
	Vec2 innerPoint8 = -1.f * outerPoint9;
	outerPoint9 += hexCorner4;
	innerPoint8 += hexCorner4;

	// hexCorner5
	Vec2 outerpoint11 = hexCorner5;
	outerpoint11.SetLength( halfThickness );
	Vec2 innerPoint10 = -1.f * outerpoint11;
	outerpoint11 += hexCorner5;
	innerPoint10 += hexCorner5;

	Vec3 zNormal = Vec3( 0.f, 0.f, 1.f );

	int hexStartIndex = ( int ) verts.size();

	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + innerPoint0 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + outerPoint1 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + innerPoint2 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + outerPoint3 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + innerPoint4 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + outerPoint5 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + innerPoint6 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + outerPoint7 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + innerPoint8 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + outerPoint9 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + innerPoint10 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + outerpoint11 ), zNormal, color ) );

	std::vector<int> hexIndexes = {
		hexStartIndex + 0, hexStartIndex + 1, hexStartIndex + 2, hexStartIndex + 2, hexStartIndex + 1, hexStartIndex + 3,
		hexStartIndex + 2, hexStartIndex + 3, hexStartIndex + 4, hexStartIndex + 4, hexStartIndex + 3, hexStartIndex + 5,
		hexStartIndex + 4, hexStartIndex + 5, hexStartIndex + 6, hexStartIndex + 6, hexStartIndex + 5, hexStartIndex + 7,
		hexStartIndex + 6, hexStartIndex + 7, hexStartIndex + 8, hexStartIndex + 8, hexStartIndex + 7, hexStartIndex + 9,
		hexStartIndex + 8, hexStartIndex + 9, hexStartIndex + 10, hexStartIndex + 10, hexStartIndex + 9, hexStartIndex + 11,
		hexStartIndex + 10, hexStartIndex + 11, hexStartIndex + 0, hexStartIndex + 0, hexStartIndex + 11, hexStartIndex + 1
	};

	indexes.insert( indexes.end(), hexIndexes.begin(), hexIndexes.end() );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForHexagon2DIndexed( std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec2 const& hexagonCenter, float circumradius, Rgba8 const& color )
{
	float const STARTING_ANGLE			 = 0.f;
	float const HEX_TRIANGLE_ARC_DEGREES = 60.f;
	Vec3		zNormal					 = Vec3( 0.f, 0.f, 1.f );
	int			hexStartIndex			 = ( int ) verts.size();

	Vec2 hexCorner0 = Vec2::MakeFromPolarDegrees( 0.f, circumradius );
	Vec2 hexCorner1 = hexCorner0.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner2 = hexCorner1.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner3 = hexCorner2.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner4 = hexCorner3.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );
	Vec2 hexCorner5 = hexCorner4.GetRotatedDegrees( HEX_TRIANGLE_ARC_DEGREES );

	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + hexCorner0 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + hexCorner1 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + hexCorner2 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + hexCorner3 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + hexCorner4 ), zNormal, color ) );
	verts.push_back( Vertex_PCUTBN( Vec3( hexagonCenter + hexCorner5 ), zNormal, color ) );

	std::vector<int> hexIndexes = {
		hexStartIndex + 0,
		hexStartIndex + 1,
		hexStartIndex + 2,
		hexStartIndex + 0,
		hexStartIndex + 2,
		hexStartIndex + 3,
		hexStartIndex + 0,
		hexStartIndex + 3,
		hexStartIndex + 4,
		hexStartIndex + 0,
		hexStartIndex + 4,
		hexStartIndex + 5,
	};

	indexes.insert( indexes.end(), hexIndexes.begin(), hexIndexes.end() );
}


// 2D wire frame

void AddVertsForWireframeAABB2D( std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float frameThickness, Rgba8 const& color )
{
	// draw 4 lines

	// 1. bottom
	AddVertsForLineSegment2D( verts, bounds.m_mins, Vec2( bounds.m_mins.x, bounds.m_maxs.y ), frameThickness, color );

	// 2. right
	AddVertsForLineSegment2D( verts, Vec2( bounds.m_mins.x, bounds.m_maxs.y ), bounds.m_maxs, frameThickness, color );

	// 3. top
	AddVertsForLineSegment2D( verts, bounds.m_maxs, Vec2( bounds.m_maxs.x, bounds.m_mins.y ), frameThickness, color );

	// 4. left
	AddVertsForLineSegment2D( verts, Vec2( bounds.m_maxs.x, bounds.m_mins.y ), bounds.m_mins, frameThickness, color );
}

void AddVertsForWireframeAABB2D( std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float z, float frameThickness, Rgba8 const& color )
{
	// draw 4 lines

	// 1. bottom
	AddVertsForLineSegment2D( verts, bounds.m_mins, Vec2( bounds.m_mins.x, bounds.m_maxs.y ), z, frameThickness, color );

	// 2. right
	AddVertsForLineSegment2D( verts, Vec2( bounds.m_mins.x, bounds.m_maxs.y ), bounds.m_maxs, z, frameThickness, color );

	// 3. top
	AddVertsForLineSegment2D( verts, bounds.m_maxs, Vec2( bounds.m_maxs.x, bounds.m_mins.y ), z, frameThickness, color );

	// 4. left
	AddVertsForLineSegment2D( verts, Vec2( bounds.m_maxs.x, bounds.m_mins.y ), bounds.m_mins, z, frameThickness, color );
}



// ---------------------------------------------------------------------------------------------------------------
// 3d transformation
// ---------------------------------------------------------------------------------------------------------------

void TransformVertexArray3D( std::vector<Vertex_PCU>& verts, Mat44 const& transform )
{
	for ( int vertIndex = 0; vertIndex < verts.size(); vertIndex++ )
	{
		Vertex_PCU& vertex = verts[ vertIndex ];
		vertex.m_position  = transform.TransformPosition3D( vertex.m_position );
	}
}

AABB2 GetVertexBounds2D( std::vector<Vertex_PCU> const& verts )
{
	AABB2 bounds;

	if ( verts.empty() )
	{
		return bounds;
	}
	else // size is at least 1
	{
		Vertex_PCU const& firstVertex = verts[ 0 ];
		Vec3 const&		  pos3D		  = firstVertex.m_position;
		Vec2			  posXY		  = Vec2( pos3D.x, pos3D.y );

		bounds.m_mins = posXY;
		bounds.m_maxs = posXY;
	}

	for ( int index = 0; index < verts.size(); index++ )
	{
		Vertex_PCU const& vertex = verts[ index ];
		Vec3 const&		  pos3D	 = vertex.m_position;
		Vec2			  posXY	 = Vec2( pos3D.x, pos3D.y );

		// mins
		if ( bounds.m_mins.x > posXY.x )
		{
			bounds.m_mins.x = posXY.x;
		}
		if ( bounds.m_mins.y > posXY.y )
		{
			bounds.m_mins.y = posXY.y;
		}
		// maxs
		if ( bounds.m_maxs.x < posXY.x )
		{
			bounds.m_maxs.x = posXY.x;
		}
		if ( bounds.m_maxs.y < posXY.y )
		{
			bounds.m_maxs.y = posXY.y;
		}
	}

	return bounds;
}

void AddVertsForQuad3D( std::vector<Vertex_PCU>& verts,
	Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft,
	Rgba8 const& color, const AABB2& UVs )
{
	// TODO: reserve the size of vector beforehand

	// triangle 1
	verts.push_back( Vertex_PCU( Vec3( bottomLeft.x, bottomLeft.y, bottomLeft.z ), color, UVs.m_mins ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight.x, bottomRight.y, bottomRight.z ), color, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( topRight.x, topRight.y, topRight.z ), color, UVs.m_maxs ) );
	// triangle 2
	verts.push_back( Vertex_PCU( Vec3( topRight.x, topRight.y, topRight.z ), color, UVs.m_maxs ) );
	verts.push_back( Vertex_PCU( Vec3( topLeft.x, topLeft.y, topLeft.z ), color, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bottomLeft.x, bottomLeft.y, bottomLeft.z ), color, UVs.m_mins ) );
}


void AddVertsForAABB3D( std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color, Vec2 uvAtMins, Vec2 uvAtMaxs )
{
	AABB2 uvs = AABB2( uvAtMins, uvAtMaxs );

	// face 3 : +x plane : back
	AddVertsForQuad3D( verts, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), color, uvs );

	// face 1 : -x plane : front
	AddVertsForQuad3D( verts, Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), color, uvs );

	// face 4 : +y plane : west
	AddVertsForQuad3D( verts, Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), color, uvs );

	// face 2 : -y_plane : east
	AddVertsForQuad3D( verts, Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), color, uvs );

	// face 6 : +z plane : top
	AddVertsForQuad3D( verts, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), color, uvs );

	// face 5 : -z_plane : bottom
	AddVertsForQuad3D( verts, Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), color, uvs );
}


void AddVertsForCylinderZ3D( std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, int numSlices, Rgba8 const& tint, AABB2 const& uvs )
{

	float height = fabsf( minMaxZ.GetMax() - minMaxZ.GetMin() );

	// 1. make disc
	float					deltaTheta = 360.f / ( float ) numSlices;
	std::vector<Vertex_PCU> discVerts;
	Vec2					radiusVec( 0.f, radius );

	// TODO: fix later (assuming uvs go from 0 to 1 always)
	float uvFraction	 = 0.5f;
	Vec2  uvDisplacement = Vec2( uvFraction, uvFraction );
	Vec2  uvCoords		 = Vec2( 0.5f, 0.f );
	Vec2  uvCenterCoords = Vec2( 0.f, 0.f );

	for ( int slice = 0; slice < numSlices; slice++ )
	{
		discVerts.push_back( Vertex_PCU( Vec3(), tint, uvCenterCoords + uvDisplacement ) );
		discVerts.push_back( Vertex_PCU( Vec3( radiusVec ), tint, uvCoords + uvDisplacement ) );
		radiusVec.RotateDegrees( deltaTheta );
		uvCoords.RotateDegrees( deltaTheta );
		discVerts.push_back( Vertex_PCU( Vec3( radiusVec ), tint, uvCoords + uvDisplacement ) );
	}

	// 2. copy and translate it to make top of cylinder
	std::vector<Vertex_PCU> topDiscVerts;
	topDiscVerts = discVerts;
	for ( int index = 0; index < topDiscVerts.size(); index++ )
	{
		Vertex_PCU& vertex	 = topDiscVerts[ index ];
		Vec3&		position = vertex.m_position;
		position.z			 = height;
	}
	verts.insert( verts.end(), topDiscVerts.begin(), topDiscVerts.end() );

	// 3. copy to make bottom of cylinder (in reverse order)
	std::vector<Vertex_PCU> bottomDiscVerts;
	bottomDiscVerts = discVerts;
	verts.insert( verts.end(), bottomDiscVerts.rbegin(), bottomDiscVerts.rend() );

	// 4. connect top and bottom disc to make the body of cylinder
	std::vector<Vertex_PCU> bodyVerts;
	radiusVec		= Vec2( 0.f, radius );
	AABB2 quadUVs	= uvs;
	float uFraction = ( uvs.m_maxs.x - uvs.m_mins.x ) / ( float ) numSlices;
	for ( int slice = 0; slice < numSlices; slice++ )
	{
		Vec3 topLeft	= Vec3( radiusVec.x, radiusVec.y, height );
		Vec3 bottomLeft = Vec3( radiusVec.x, radiusVec.y, 0.f );
		radiusVec.RotateDegrees( deltaTheta );
		Vec3 topRight	 = Vec3( radiusVec.x, radiusVec.y, height );
		Vec3 bottomRight = Vec3( radiusVec.x, radiusVec.y, 0.f );

		quadUVs.m_maxs.x = quadUVs.m_mins.x + uFraction;
		AddVertsForQuad3D( bodyVerts, bottomLeft, bottomRight, topRight, topLeft, tint, quadUVs );
		quadUVs.m_mins.x = quadUVs.m_maxs.x;
	}
	verts.insert( verts.end(), bodyVerts.begin(), bodyVerts.end() );

	// 5. translate all vertexes to centerXY & minZ
	Vec3 translation = Vec3( centerXY.x, centerXY.y, minMaxZ.GetMin() );
	for ( int index = 0; index < verts.size(); index++ )
	{
		Vertex_PCU& vertex	 = verts[ index ];
		Vec3&		position = vertex.m_position;
		position += translation;
	}
}


void AddVertsForCylinder3D( std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color, AABB2 const& uvs, int numSlices )
{
	UNUSED( uvs );
	std::vector<Vertex_PCU> cylinderVerts;

	// 1. construct cylinder flat lying along the x axis, starting at the origin
	Vec3  displacement = end - start;
	float length	   = displacement.GetLength();

	// Bottom cap
	// construct a circle in y-z plane
	float deltaTheta = 360.f / ( float ) numSlices;
	Vec2  centerXY	 = Vec2::ZERO;
	Vec2  radiusXY	 = Vec2( radius, 0.f );
	for ( int slice = 0; slice < numSlices; slice++ )
	{
		Vec3 centerXYZ;
		centerXYZ.x = 0.f;
		centerXYZ.y = centerXY.y;
		centerXYZ.z = centerXY.x;
		cylinderVerts.push_back( Vertex_PCU( centerXYZ, color ) );

		Vec3 radiusXYZ;
		radiusXYZ.x = 0.f;
		radiusXYZ.y = radiusXY.y;
		radiusXYZ.z = radiusXY.x;
		cylinderVerts.push_back( Vertex_PCU( radiusXYZ, color ) );

		radiusXY.RotateDegrees( deltaTheta );
		radiusXYZ.x = 0.f;
		radiusXYZ.y = radiusXY.y;
		radiusXYZ.z = radiusXY.x;
		cylinderVerts.push_back( Vertex_PCU( radiusXYZ, color ) );
	}

	// top cap
	for ( int slice = 0; slice < numSlices; slice++ )
	{
		Vec3 centerXYZ;
		centerXYZ.x = length;
		centerXYZ.y = centerXY.x;
		centerXYZ.z = centerXY.y;
		cylinderVerts.push_back( Vertex_PCU( centerXYZ, color ) );

		Vec3 radiusXYZ;
		radiusXYZ.x = length;
		radiusXYZ.y = radiusXY.x;
		radiusXYZ.z = radiusXY.y;
		cylinderVerts.push_back( Vertex_PCU( radiusXYZ, color ) );

		radiusXY.RotateDegrees( deltaTheta );
		radiusXYZ.x = length;
		radiusXYZ.y = radiusXY.x;
		radiusXYZ.z = radiusXY.y;
		cylinderVerts.push_back( Vertex_PCU( radiusXYZ, color ) );
	}

	// wrap around
	radiusXY = Vec2( radius, 0.f );
	for ( int slice = 0; slice < numSlices; slice++ )
	{
		Vec3 bottomLeft = Vec3( 0.f, radiusXY.x, radiusXY.y );
		Vec3 topLeft	= Vec3( length, bottomLeft.y, bottomLeft.z );

		radiusXY.RotateDegrees( deltaTheta );
		Vec3 bottomRight = Vec3( 0.f, radiusXY.x, radiusXY.y );
		Vec3 topRight	 = Vec3( length, bottomRight.y, bottomRight.z );

		cylinderVerts.push_back( Vertex_PCU( bottomLeft, color ) );
		cylinderVerts.push_back( Vertex_PCU( bottomRight, color ) );
		cylinderVerts.push_back( Vertex_PCU( topRight, color ) );

		cylinderVerts.push_back( Vertex_PCU( bottomLeft, color ) );
		cylinderVerts.push_back( Vertex_PCU( topRight, color ) );
		cylinderVerts.push_back( Vertex_PCU( topLeft, color ) );
	}

	// 2. create transformation matrix
	Mat44 matrix;
	Vec3  translation = start;
	Vec3  iForward	  = end - start;
	iForward.Normalize();
	Vec3 worldUp = Vec3( 0.f, 0.f, 1.f );
	Vec3 jLeft	 = CrossProduct3D( worldUp, iForward );

	if ( jLeft != Vec3( 0.f, 0.f, 0.f ) )
	{
		jLeft.Normalize();
	}
	else
	{
		Vec3 worldForward = Vec3( 1.f, 0.f, 0.f );
		jLeft			  = CrossProduct3D( iForward, worldForward );
		jLeft.Normalize();
	}

	Vec3 kUp;
	kUp = CrossProduct3D( iForward, jLeft );
	kUp.Normalize();

	matrix.SetTranslation3D( start );
	matrix.SetIJK3D( iForward, jLeft, kUp );

	// 3. transform all points
	for ( int index = 0; index < cylinderVerts.size(); index++ )
	{
		Vertex_PCU& vertex		= cylinderVerts[ index ];
		Vec3		oldPosition = vertex.m_position;

		Vec3 transformedPosition = matrix.TransformPosition3D( oldPosition );
		vertex.m_position		 = transformedPosition;
	}

	// append to the original verts
	verts.insert( verts.end(), cylinderVerts.begin(), cylinderVerts.end() );
}


constexpr float CROSS_PRODUCT_EPSILON = 0.0001f;
void AddVertsForCone3D( std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color, AABB2 const& uvs, int numSlices )
{
	UNUSED( uvs );

	std::vector<Vertex_PCU> coneVerts;

	// 1. construct a disc
	// 1.1 make a disc in xy plane

	std::vector<Vertex_PCU> discXY;
	Vec2					centerXY   = Vec2( 0.f, 0.f );
	Vec2					radiusXY   = Vec2( radius, 0.f );
	float					deltaTheta = 360.f / numSlices;

	for ( int slice = 0; slice < numSlices; slice++ )
	{
		discXY.push_back( Vertex_PCU( Vec3( centerXY ), color ) );
		discXY.push_back( Vertex_PCU( Vec3( radiusXY ), color ) );
		radiusXY.RotateDegrees( deltaTheta );
		discXY.push_back( Vertex_PCU( Vec3( radiusXY ), color ) );
	}

	// 1.2 move the disc to yz plane

	Mat44 identity;
	Mat44 xy_to_yz_matrix;
	xy_to_yz_matrix.SetIJK3D( identity.GetKBasis3D(), identity.GetJBasis3D(), identity.GetIBasis3D() );
	std::vector<Vertex_PCU> discYZ = discXY;

	for ( int discVertIndex = 0; discVertIndex < discYZ.size(); discVertIndex++ )
	{
		Vertex_PCU& discVertex = discYZ[ discVertIndex ];
		discVertex.m_position  = xy_to_yz_matrix.TransformPosition3D( discVertex.m_position );
	}

	coneVerts.insert( coneVerts.end(), discYZ.begin(), discYZ.end() );


	// 2. construct the wrap around part
	Vec3					displacement = end - start;
	float					length		 = displacement.GetLength();
	Vec3					topPoint	 = Vec3( length, 0.f, 0.f );
	std::vector<Vertex_PCU> wrapVerts;

	for ( int discVertIndex = int( discYZ.size() - 1 ); discVertIndex >= 0; discVertIndex-- )
	{
		// make every 3rd vertex the top point
		if ( discVertIndex % 3 == 0 )
		{
			wrapVerts.push_back( Vertex_PCU( topPoint, color ) );
		}
		else
		{
			wrapVerts.push_back( discYZ[ discVertIndex ] );
		}
	}

	coneVerts.insert( coneVerts.end(), wrapVerts.begin(), wrapVerts.end() );


	// 3. Create the transformation matrix
	Mat44 transformationMatrix;
	Vec3  kUp;
	Vec3  jLeft;
	Vec3  iForward = end - start;
	iForward.Normalize();

	// 1. calculate jLeft
	Vec3 worldUp = Vec3( 0.f, 0.f, 1.f );
	jLeft		 = CrossProduct3D( worldUp, iForward );
	if ( fabsf( jLeft.x ) <= CROSS_PRODUCT_EPSILON &&
		 fabsf( jLeft.y ) <= CROSS_PRODUCT_EPSILON &&
		 fabsf( jLeft.z ) <= CROSS_PRODUCT_EPSILON )
	{
		Vec3 worldFowrad = Vec3( 1.f, 0.f, 0.f );
		jLeft			 = CrossProduct3D( iForward, worldFowrad );
	}
	jLeft.Normalize();
	kUp = CrossProduct3D( iForward, jLeft );
	kUp.Normalize();

	// rotation
	transformationMatrix.SetIJK3D( iForward, jLeft, kUp );

	// translation
	transformationMatrix.SetTranslation3D( start );


	// 4. transform all cone verts
	for ( int coneVertsIndex = 0; coneVertsIndex < coneVerts.size(); coneVertsIndex++ )
	{
		Vertex_PCU& coneVertex = coneVerts[ coneVertsIndex ];
		coneVertex.m_position  = transformationMatrix.TransformPosition3D( coneVertex.m_position );
	}

	verts.insert( verts.end(), coneVerts.begin(), coneVerts.end() );
}


void AddVertsForSphere3D( std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& uvs, int numLatitudeSlices )
{
	float numLongitudeSlices	= numLatitudeSlices * 2.f;
	float deltaLongitudeDegrees = 360.f / numLongitudeSlices;
	float deltaLatitudeDegrees	= 180.f / numLatitudeSlices;

	float deltaU = ( uvs.m_maxs.x - uvs.m_mins.x ) / numLongitudeSlices;
	float deltaV = ( uvs.m_maxs.y - uvs.m_mins.y ) / numLatitudeSlices;

	float u = 0.f;
	for ( float longitudeDegrees = 0.f; longitudeDegrees < 360.f; longitudeDegrees += deltaLongitudeDegrees )
	{
		float v = 0.f;
		for ( float latitudeDegrees = -90.f; latitudeDegrees < 90.f; latitudeDegrees += deltaLatitudeDegrees )
		{
			Vec3 bottomLeft	 = center + Vec3::MakeFromPolarDegrees( latitudeDegrees, longitudeDegrees, radius );
			Vec3 topLeft	 = center + Vec3::MakeFromPolarDegrees( latitudeDegrees + deltaLatitudeDegrees, longitudeDegrees, radius );
			Vec3 bottomRight = center + Vec3::MakeFromPolarDegrees( latitudeDegrees, longitudeDegrees + deltaLongitudeDegrees, radius );
			Vec3 topRight	 = center + Vec3::MakeFromPolarDegrees( latitudeDegrees + deltaLatitudeDegrees, longitudeDegrees + deltaLongitudeDegrees, radius );

			verts.push_back( Vertex_PCU( bottomLeft, color, Vec2( u, v ) ) );
			verts.push_back( Vertex_PCU( bottomRight, color, Vec2( u + deltaU, v ) ) );
			verts.push_back( Vertex_PCU( topRight, color, Vec2( u + deltaU, v + deltaV ) ) );
			verts.push_back( Vertex_PCU( topRight, color, Vec2( u + deltaU, v + deltaV ) ) );
			verts.push_back( Vertex_PCU( topLeft, color, Vec2( u, v + deltaV ) ) );
			verts.push_back( Vertex_PCU( bottomLeft, color, Vec2( u, v ) ) );

			v += deltaV;
		}
		u += deltaU;
	}
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForArrow3D( std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color, AABB2 const& uvs, int numSlices )
{
	Vec3  displacement	 = end - start;
	Vec3  direction		 = displacement.GetNormalized();
	float length		 = displacement.GetLength();
	float cylinderLength = 0.7f * length;
	float coneLength	 = 10.f * radius;
	float conreRadius	 = 3.f * radius;

	Vec3 cylinderStart = start;
	Vec3 cylinderEnd   = cylinderStart + ( direction * cylinderLength );
	Vec3 coneStart	   = cylinderEnd;
	Vec3 coneEnd	   = coneStart + ( direction * coneLength );

	AddVertsForCone3D( verts, coneStart, coneEnd, conreRadius, color, uvs, numSlices );
	AddVertsForCylinder3D( verts, cylinderStart, cylinderEnd, radius, color, uvs, numSlices );
}



// 3d add verts using index buffers ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AddVertsForQuad3D( std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, const AABB2& UVs )
{
	unsigned int lastVertexIndex = ( unsigned int ) vertexes.size();

	// add verts
	vertexes.push_back( Vertex_PCU( Vec3( bottomLeft.x, bottomLeft.y, bottomLeft.z ), color, UVs.m_mins ) );
	vertexes.push_back( Vertex_PCU( Vec3( bottomRight.x, bottomRight.y, bottomRight.z ), color, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	vertexes.push_back( Vertex_PCU( Vec3( topRight.x, topRight.y, topRight.z ), color, UVs.m_maxs ) );
	vertexes.push_back( Vertex_PCU( Vec3( topLeft.x, topLeft.y, topLeft.z ), color, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );

	// add indexes

	std::vector<unsigned int> quadIndexs = { 0, 1, 2, 0, 2, 3 };
	for ( int index = 0; index < quadIndexs.size(); index++ )
	{
		quadIndexs[ index ] += lastVertexIndex;
	}
	indexes.insert( indexes.end(), quadIndexs.begin(), quadIndexs.end() );
}


// 3d add verts using index buffers and Vertex PCUTBN -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AddVertsForQuad3DPCUTBN( std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, const AABB2& UVs )
{
	unsigned int lastVertexIndex = ( unsigned int ) vertexes.size();

	Vertex_PCUTBN bottomLeftVertex;
	bottomLeftVertex.m_position	   = Vec3( bottomLeft.x, bottomLeft.y, bottomLeft.z );
	bottomLeftVertex.m_normal	   = CrossProduct3D( bottomRight - bottomLeft, topLeft - bottomLeft ).GetNormalized();
	bottomLeftVertex.m_color	   = color;
	bottomLeftVertex.m_uvTexCoords = UVs.m_mins;

	Vertex_PCUTBN bottomRightVertex;
	bottomRightVertex.m_position	= Vec3( bottomRight.x, bottomRight.y, bottomRight.z );
	bottomRightVertex.m_normal		= CrossProduct3D( topRight - bottomRight, bottomLeft - bottomRight ).GetNormalized();
	bottomRightVertex.m_color		= color;
	bottomRightVertex.m_uvTexCoords = Vec2( UVs.m_maxs.x, UVs.m_mins.y );

	Vertex_PCUTBN topRightVertex;
	topRightVertex.m_position	 = Vec3( topRight.x, topRight.y, topRight.z );
	topRightVertex.m_normal		 = CrossProduct3D( topLeft - topRight, bottomRight - topRight ).GetNormalized();
	topRightVertex.m_color		 = color;
	topRightVertex.m_uvTexCoords = UVs.m_maxs;

	Vertex_PCUTBN topLeftVertex;
	topLeftVertex.m_position	= Vec3( topLeft.x, topLeft.y, topLeft.z );
	topLeftVertex.m_normal		= CrossProduct3D( bottomLeft - topLeft, topRight - topLeft ).GetNormalized();
	topLeftVertex.m_color		= color;
	topLeftVertex.m_uvTexCoords = Vec2( UVs.m_mins.x, UVs.m_maxs.y );

	// add verts
	vertexes.push_back( bottomLeftVertex );
	vertexes.push_back( bottomRightVertex );
	vertexes.push_back( topRightVertex );
	vertexes.push_back( topLeftVertex );

	// add indexes

	std::vector<unsigned int> quadIndexs = { 0, 1, 2, 0, 2, 3 };
	for ( int index = 0; index < quadIndexs.size(); index++ )
	{
		quadIndexs[ index ] += lastVertexIndex;
	}
	indexes.insert( indexes.end(), quadIndexs.begin(), quadIndexs.end() );
}

void AddVertsForQuad3DPCUTBN( std::vector<Vertex_PCUTBN>& vertexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, const AABB2& UVs )
{
	Vertex_PCUTBN bottomLeftVertex;
	bottomLeftVertex.m_position	   = Vec3( bottomLeft.x, bottomLeft.y, bottomLeft.z );
	bottomLeftVertex.m_normal	   = CrossProduct3D( bottomRight - bottomLeft, topLeft - bottomLeft ).GetNormalized();
	bottomLeftVertex.m_color	   = color;
	bottomLeftVertex.m_uvTexCoords = UVs.m_mins;

	Vertex_PCUTBN bottomRightVertex;
	bottomRightVertex.m_position	= Vec3( bottomRight.x, bottomRight.y, bottomRight.z );
	bottomRightVertex.m_normal		= CrossProduct3D( topRight - bottomRight, bottomLeft - bottomRight ).GetNormalized();
	bottomRightVertex.m_color		= color;
	bottomRightVertex.m_uvTexCoords = Vec2( UVs.m_maxs.x, UVs.m_mins.y );

	Vertex_PCUTBN topRightVertex;
	topRightVertex.m_position	 = Vec3( topRight.x, topRight.y, topRight.z );
	topRightVertex.m_normal		 = CrossProduct3D( topLeft - topRight, bottomRight - topRight ).GetNormalized();
	topRightVertex.m_color		 = color;
	topRightVertex.m_uvTexCoords = UVs.m_maxs;

	Vertex_PCUTBN topLeftVertex;
	topLeftVertex.m_position	= Vec3( topLeft.x, topLeft.y, topLeft.z );
	topLeftVertex.m_normal		= CrossProduct3D( bottomLeft - topLeft, topRight - topLeft ).GetNormalized();
	topLeftVertex.m_color		= color;
	topLeftVertex.m_uvTexCoords = Vec2( UVs.m_mins.x, UVs.m_maxs.y );

	// add verts
	vertexes.push_back( bottomLeftVertex );
	vertexes.push_back( bottomRightVertex );
	vertexes.push_back( topRightVertex );

	vertexes.push_back( bottomLeftVertex );
	vertexes.push_back( topRightVertex );
	vertexes.push_back( topLeftVertex );
}

void AddVertsForRoundedQuad3DPCUTBN( std::vector<Vertex_PCUTBN>& vertexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, const AABB2& UVs )
{
	Vec3 rightNormal = ( bottomRight - bottomLeft ).GetNormalized();
	Vec3 leftNormal	 = -1.f * rightNormal;

	Vertex_PCUTBN bottomLeftVertex;
	bottomLeftVertex.m_position	   = Vec3( bottomLeft.x, bottomLeft.y, bottomLeft.z );
	bottomLeftVertex.m_normal	   = leftNormal;
	bottomLeftVertex.m_color	   = color;
	bottomLeftVertex.m_uvTexCoords = UVs.m_mins;

	Vertex_PCUTBN bottomRightVertex;
	bottomRightVertex.m_position	= Vec3( bottomRight.x, bottomRight.y, bottomRight.z );
	bottomRightVertex.m_normal		= rightNormal;
	bottomRightVertex.m_color		= color;
	bottomRightVertex.m_uvTexCoords = Vec2( UVs.m_maxs.x, UVs.m_mins.y );

	Vertex_PCUTBN topRightVertex;
	topRightVertex.m_position	 = Vec3( topRight.x, topRight.y, topRight.z );
	topRightVertex.m_normal		 = rightNormal;
	topRightVertex.m_color		 = color;
	topRightVertex.m_uvTexCoords = UVs.m_maxs;

	Vertex_PCUTBN topLeftVertex;
	topLeftVertex.m_position	= Vec3( topLeft.x, topLeft.y, topLeft.z );
	topLeftVertex.m_normal		= leftNormal;
	topLeftVertex.m_color		= color;
	topLeftVertex.m_uvTexCoords = Vec2( UVs.m_mins.x, UVs.m_maxs.y );

	Vec3 up			  = topLeft - bottomLeft;
	Vec3 middleNormal = CrossProduct3D( rightNormal, up ).GetNormalized();

	Vertex_PCUTBN bottomMiddleVertex;
	bottomMiddleVertex.m_position	 = ( ( bottomLeft + bottomRight ) * 0.5f );
	bottomMiddleVertex.m_normal		 = middleNormal;
	bottomMiddleVertex.m_color		 = color;
	bottomMiddleVertex.m_uvTexCoords = Vec2( ( UVs.m_maxs.x + UVs.m_mins.x ) * 0.5f, UVs.m_mins.y );

	Vertex_PCUTBN topMiddleVertex;
	topMiddleVertex.m_position	  = ( ( topLeft + topRight ) * 0.5f );
	topMiddleVertex.m_normal	  = middleNormal;
	topMiddleVertex.m_color		  = color;
	topMiddleVertex.m_uvTexCoords = Vec2( ( UVs.m_maxs.x + UVs.m_mins.x ) * 0.5f, UVs.m_maxs.y );

	// add verts
	vertexes.push_back( bottomLeftVertex );
	vertexes.push_back( bottomMiddleVertex );
	vertexes.push_back( topMiddleVertex );
	vertexes.push_back( bottomLeftVertex );
	vertexes.push_back( topMiddleVertex );
	vertexes.push_back( topLeftVertex );

	vertexes.push_back( bottomMiddleVertex );
	vertexes.push_back( bottomRightVertex );
	vertexes.push_back( topRightVertex );
	vertexes.push_back( bottomMiddleVertex );
	vertexes.push_back( topRightVertex );
	vertexes.push_back( topMiddleVertex );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForAABB3DPCUTBN( std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, AABB3 const& bounds, Rgba8 const& color, AABB2 uvs )
{
	// face 3 : +x plane : back
	AddVertsForQuad3DPCUTBN( verts, indexes, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), color, uvs );

	// face 1 : -x plane : front
	AddVertsForQuad3DPCUTBN( verts, indexes, Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), color, uvs );

	// face 4 : +y plane : west
	AddVertsForQuad3DPCUTBN( verts, indexes, Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), color, uvs );

	// face 2 : -y_plane : east
	AddVertsForQuad3DPCUTBN( verts, indexes, Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), color, uvs );

	// face 6 : +z plane : top
	AddVertsForQuad3DPCUTBN( verts, indexes, Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), color, uvs );

	// face 5 : -z_plane : bottom
	AddVertsForQuad3DPCUTBN( verts, indexes, Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), color, uvs );
}


//----------------------------------------------------------------------------------------------------------
// void AddVertsForSphere3DPCUTBN( std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& uvs, int numLatitudeSlices )
//{
//
//}


void AddVertsForWireframeAABB3D( std::vector<Vertex_PCU>& verts, AABB3 const& bounds, float frameThickness, Rgba8 const& color )
{
	// bottom
	Vec3 bottomFrontLeft  = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 bottomFrontRight = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 bottomBackRight  = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
	Vec3 bottomBackLeft	  = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
	AddVertsForWireframeQuad3D( verts, bottomFrontLeft, bottomFrontRight, bottomBackRight, bottomBackLeft, frameThickness, color );

	// top
	Vec3 topFrontLeft  = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );
	Vec3 topFrontRight = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
	Vec3 topBackRight  = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );
	Vec3 topBackLeft   = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );
	AddVertsForWireframeQuad3D( verts, topFrontLeft, topFrontRight, topBackRight, topBackLeft, frameThickness, color );

	// sides
	AddVertsForCylinder3D( verts, bottomFrontLeft, topFrontLeft, frameThickness, color );
	AddVertsForCylinder3D( verts, bottomFrontRight, topFrontRight, frameThickness, color );
	AddVertsForCylinder3D( verts, bottomBackRight, topBackRight, frameThickness, color );
	AddVertsForCylinder3D( verts, bottomBackLeft, topBackLeft, frameThickness, color );
}

void AddVertsForWireframeQuad3D( std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, float frameThickness, Rgba8 const& color )
{
	// use addvertsforcylinder3d to add 4 lines
	AddVertsForCylinder3D( verts, bottomLeft, bottomRight, frameThickness, color );
	AddVertsForCylinder3D( verts, bottomRight, topRight, frameThickness, color );
	AddVertsForCylinder3D( verts, topRight, topLeft, frameThickness, color );
	AddVertsForCylinder3D( verts, topLeft, bottomLeft, frameThickness, color );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForLineUsingLineList3D( std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, Rgba8 const& color )
{
	verts.push_back( Vertex_PCU( start, color ) );
	verts.push_back( Vertex_PCU( end, color ) );
}


//----------------------------------------------------------------------------------------------------------
void CalculateTangetSpaceBasisVectorForVertex_PCUTBN( std::vector<Vertex_PCUTBN>& outCpuVerts, std::vector<unsigned int> const& indexes )
{
	// 1. calculate tangents and bi-tangents for each triangle
	for ( int index = 0; index < indexes.size(); index += 3 )
	{
		// get indexes
		unsigned int index0 = indexes[ index ];
		unsigned int index1 = indexes[ index + 1 ];
		unsigned int index2 = indexes[ index + 2 ];

		// get points
		Vec3 const& point0 = outCpuVerts[ index0 ].m_position;
		Vec3 const& point1 = outCpuVerts[ index1 ].m_position;
		Vec3 const& point2 = outCpuVerts[ index2 ].m_position;

		// get uvs
		Vec2 const& uv0 = outCpuVerts[ index0 ].m_uvTexCoords;
		Vec2 const& uv1 = outCpuVerts[ index1 ].m_uvTexCoords;
		Vec2 const& uv2 = outCpuVerts[ index2 ].m_uvTexCoords;

		// get edges
		Vec3  edge1 = point1 - point0;
		Vec3  edge2 = point2 - point0;
		float x1	= uv1.x - uv0.x;
		float x2	= uv2.x - uv0.x;
		float y1	= uv1.y - uv0.y;
		float y2	= uv2.y - uv0.y;

		// calculate tangent and bi-tangent
		float r			= 1.f / ( x1 * y2 - x2 * y1 );
		Vec3  tangent	= ( edge1 * y2 - edge2 * y1 ) * r;
		Vec3  biTangent = ( edge2 * x1 - edge1 * x2 ) * r;

		// add to the vertex
		outCpuVerts[ index0 ].m_tangent += tangent;
		outCpuVerts[ index1 ].m_tangent += tangent;
		outCpuVerts[ index2 ].m_tangent += tangent;
		outCpuVerts[ index0 ].m_bitangent += biTangent;
		outCpuVerts[ index1 ].m_bitangent += biTangent;
		outCpuVerts[ index2 ].m_bitangent += biTangent;
	}

	// 2. orthonormalize the tangent and bi-tangent and calculate handedness
	for ( int index = 0; index < outCpuVerts.size(); index++ )
	{
		Vertex_PCUTBN& vertex	 = outCpuVerts[ index ];
		Vec3&		   tangent	 = vertex.m_tangent;
		Vec3&		   biTangent = vertex.m_bitangent;
		Vec3&		   normal	 = vertex.m_normal;

		// Gram-Schmidt orthogonalize
		tangent = tangent - ( DotProduct3D( normal, tangent ) * normal );
		tangent.Normalize();

		// calculate handedness
		float handedness = ( DotProduct3D( CrossProduct3D( normal, tangent ), biTangent ) < 0.f ) ? -1.f : 1.f;
		biTangent		 = CrossProduct3D( normal, tangent ) * handedness;
	}
}



//----------------------------------------------------------------------------------------------------------
void AddVertsForConvexPolly2DIndexed( std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, ConvexPolly2 const& convexPolly2, Rgba8 const& color )
{
	std::vector<Vec2> pollyPoints = convexPolly2.GetCounterClockWiseOrderedPoints();
	for ( int pointIndex = 0; pointIndex < pollyPoints.size(); pointIndex++ )
	{
		Vertex_PCU vertex;
		vertex.m_color	  = color;
		vertex.m_position = Vec3( pollyPoints[ pointIndex ] );

		verts.push_back( vertex );
	}

	size_t totalTriangles = pollyPoints.size() - 2;
	for ( int traingleNum = 0; traingleNum < totalTriangles; traingleNum++ )
	{
		int firstIndex	= 0;
		int secondIndex = traingleNum + 1;
		int thirdindex	= traingleNum + 2;

		indexes.push_back( firstIndex );
		indexes.push_back( secondIndex );
		indexes.push_back( thirdindex );
	}
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForPlane3D( std::vector<Vertex_PCU>& verts, Plane3 const& plane3D, float width, float height, Rgba8 const& color, AABB2 const& uvs )
{


	UNUSED( uvs );

	Vec3 worldUp  = Vec3( 0.f, 0.f, 1.f );
	Vec3 worldFwd = Vec3( 1.f, 0.f, 0.f );

	Vec3 planeiFwd;
	if ( plane3D.m_normal == worldUp || plane3D.m_normal == ( -1.f * worldUp ) )
	{
		planeiFwd = CrossProduct3D( worldFwd, plane3D.m_normal );
	}
	else
	{
		planeiFwd = CrossProduct3D( worldUp, plane3D.m_normal );
	}
	planeiFwd.Normalize();

	Vec3 planejLeft = CrossProduct3D( planeiFwd, plane3D.m_normal );
	planejLeft.Normalize();

	// add verts for both sides of the plane
	Vec3 planeCenter = ( plane3D.m_normal * plane3D.m_distanceFromOrigin );
	planeiFwd		 = /*planeCenter + */ ( planeiFwd * width );
	planejLeft		 = /*planeCenter +*/ ( planejLeft * height );

	Vec3 bottomLeft	 = planeCenter + ( planejLeft - planeiFwd );
	Vec3 bottomRight = planeCenter + ( planeiFwd + planejLeft );
	Vec3 topRight	 = planeCenter + ( planeiFwd - planejLeft );
	Vec3 topLeft	 = planeCenter + ( -1.f * planeiFwd - planejLeft );

	Rgba8 planeColor = color;
	verts.push_back( Vertex_PCU( bottomLeft, planeColor ) );
	verts.push_back( Vertex_PCU( bottomRight, planeColor ) );
	verts.push_back( Vertex_PCU( topRight, planeColor ) );
	verts.push_back( Vertex_PCU( bottomLeft, planeColor ) );
	verts.push_back( Vertex_PCU( topRight, planeColor ) );
	verts.push_back( Vertex_PCU( topLeft, planeColor ) );

	// flipped side, clock wise- winding order
	Rgba8 backSideColor = planeColor;
	backSideColor.r		= 80;
	verts.push_back( Vertex_PCU( bottomLeft, backSideColor ) );
	verts.push_back( Vertex_PCU( topRight, backSideColor ) );
	verts.push_back( Vertex_PCU( bottomRight, backSideColor ) );
	verts.push_back( Vertex_PCU( bottomLeft, backSideColor ) );
	verts.push_back( Vertex_PCU( topLeft, backSideColor ) );
	verts.push_back( Vertex_PCU( topRight, backSideColor ) );


	// std::vector<Vertex_PCU> verts;
	AddVertsForArrow3D( verts, planeCenter, planeCenter + plane3D.m_normal, 0.03f, Rgba8::WHITE );
	AddVertsForArrow3D( verts, planeCenter, planeCenter + planeiFwd, 0.03f, Rgba8::GRAY );
	AddVertsForArrow3D( verts, planeCenter, planeCenter + planejLeft, 0.03f, Rgba8::MAGENTA );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForOBB3D( std::vector<Vertex_PCU>& verts, OBB3 const& obb3D, Rgba8 const& color, AABB2 const& uvs )
{
	UNUSED( uvs );

	Vec3 iBasisNormal = obb3D.m_iBasisNormal;
	Vec3 jBasisNormal = obb3D.m_jBasisNormal;
	Vec3 kBasisNormal = CrossProduct3D( iBasisNormal, jBasisNormal );
	kBasisNormal.Normalize();

	float hx = obb3D.m_dimensions.x; // half length
	float hy = obb3D.m_dimensions.y; // half width
	float hz = obb3D.m_dimensions.z; // half height

	// points
	Vec3 upBottomLeft  = obb3D.m_center + ( -1.f * iBasisNormal * hx ) + ( jBasisNormal * hy ) + ( kBasisNormal * hz );
	Vec3 upBottomRight = obb3D.m_center + ( -1.f * iBasisNormal * hx ) + ( -1.f * jBasisNormal * hy ) + ( kBasisNormal * hz );
	Vec3 upTopLeft	   = obb3D.m_center + ( iBasisNormal * hx ) + ( jBasisNormal * hy ) + ( kBasisNormal * hz );
	Vec3 upTopRight	   = obb3D.m_center + ( iBasisNormal * hx ) + ( -1.f * jBasisNormal * hy ) + ( kBasisNormal * hz );

	Vec3 downBottomLeft	 = obb3D.m_center + ( -1.f * iBasisNormal * hx ) + ( jBasisNormal * hy ) + ( -1.f * kBasisNormal * hz );
	Vec3 downBottomRight = obb3D.m_center + ( -1.f * iBasisNormal * hx ) + ( -1.f * jBasisNormal * hy ) + ( -1.f * kBasisNormal * hz );
	Vec3 downTopLeft	 = obb3D.m_center + ( iBasisNormal * hx ) + ( jBasisNormal * hy ) + ( -1.f * kBasisNormal * hz );
	Vec3 downTopRight	 = obb3D.m_center + ( iBasisNormal * hx ) + ( -1.f * jBasisNormal * hy ) + ( -1.f * kBasisNormal * hz );


	// sides
	// top
	AddVertsForQuad3D( verts, upBottomLeft, upBottomRight, upTopRight, upTopLeft, Rgba8::PALE_BLUE );

	// bottom
	AddVertsForQuad3D( verts, downBottomRight, downBottomLeft, downTopLeft, downTopRight, color );

	// right
	AddVertsForQuad3D( verts, downBottomRight, downTopRight, upTopRight, upBottomRight, color );

	// left
	AddVertsForQuad3D( verts, downTopLeft, downBottomLeft, upBottomLeft, upTopLeft, Rgba8::SOFT_GREEN );

	// front
	AddVertsForQuad3D( verts, downTopRight, downTopLeft, upTopLeft, upTopRight, Rgba8::SOFT_RED );

	// back
	AddVertsForQuad3D( verts, downBottomLeft, downBottomRight, upBottomRight, upBottomLeft, color );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForWireframeConvexPolly2DIndexed( std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, ConvexPolly2 const& convexPolly2, float frameThickness, Rgba8 const& color )
{

	std::vector<Vec2> pollyPoints = convexPolly2.GetCounterClockWiseOrderedPoints();
	for ( int index = 0; index < pollyPoints.size() - 1; index++ )
	{
		Vec2 startPoint = pollyPoints[ index ];
		Vec2 endpoint	= pollyPoints[ index + 1 ];

		AddVertsForLineSegment2DIndexed( verts, indexes, startPoint, endpoint, frameThickness, color );
	}

	// joining end to start
	Vec2 startPoint = pollyPoints[ pollyPoints.size() - 1 ];
	Vec2 endpoint	= pollyPoints[ 0 ];

	AddVertsForLineSegment2DIndexed( verts, indexes, startPoint, endpoint, frameThickness, color );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2DIndexed( std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec2 const& startPoint, Vec2 const& endpoint, float thickness, Rgba8 const& color )
{
	// 1. calculations
	float halfThickness = thickness * 0.5f;
	float ninteyDegrees = 90.0f;
	float deltaDegrees	= 45.0f;

	float orientationDegrees = ( Vec2( endpoint ) - Vec2( startPoint ) ).GetOrientationDegrees();
	float R					 = GetDistance2D( startPoint, endpoint );

	float xTopLeft = R * CosDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );
	float yTopLeft = R * SinDegrees( orientationDegrees + ( ninteyDegrees + deltaDegrees ) );

	float xBottomLeft = R * CosDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );
	float yBottomLeft = R * SinDegrees( orientationDegrees - ( ninteyDegrees + deltaDegrees ) );

	float xTopRight = R * CosDegrees( orientationDegrees + deltaDegrees );
	float yTopRight = R * SinDegrees( orientationDegrees + deltaDegrees );

	float xBottomRight = R * CosDegrees( orientationDegrees - deltaDegrees );
	float yBottomRight = R * SinDegrees( orientationDegrees - deltaDegrees );

	Vec2 topLeft( xTopLeft, yTopLeft );
	Vec2 topRight( xTopRight, yTopRight );
	Vec2 bottomRight( xBottomRight, yBottomRight );
	Vec2 bottomLeft( xBottomLeft, yBottomLeft );

	topLeft.SetLength( halfThickness );
	topRight.SetLength( halfThickness );
	bottomRight.SetLength( halfThickness );
	bottomLeft.SetLength( halfThickness );

	topLeft += startPoint;
	topRight += endpoint;
	bottomRight += endpoint;
	bottomLeft += startPoint;

	unsigned int previousIndex = static_cast<unsigned int>( verts.size() );

	// 2. add vertices
	verts.push_back( Vertex_PCU( Vec3( bottomLeft ), color ) );
	verts.push_back( Vertex_PCU( Vec3( bottomRight ), color ) );
	verts.push_back( Vertex_PCU( Vec3( topRight ), color ) );
	verts.push_back( Vertex_PCU( Vec3( topLeft ), color ) );

	// 3. add indexes
	std::vector<unsigned int> lineIndexes = { previousIndex + 0, previousIndex + 1, previousIndex + 2, previousIndex + 0, previousIndex + 2, previousIndex + 3 };
	indexes.insert( indexes.end(), lineIndexes.begin(), lineIndexes.end() );
}


//----------------------------------------------------------------------------------------------------------
void AddVertsForSectorRingWithArrow3D( std::vector<Vertex_PCU>& verts, Vec3 const& planeNormal, Vec3 const& sectorCenter, float sectorRadius, float sectorStartAngle, float sectorEndAngle, float ringThickness, float arrowRadius, int numSlices, Rgba8 color )
{
	std::vector<Vertex_PCU> localVerts;
	Vec3					worldUp = Vec3( 0.f, 0.f, 1.f );

	float totalSectorAngle = sectorEndAngle - sectorStartAngle;
	float stepAngle		   = totalSectorAngle / numSlices;

	float segmentStartAngle = sectorStartAngle;

	// first segment
	Vec3 stepStartVector = Vec3( sectorRadius, 0.f, 0.f );

	Quaternion rotorForStartPoint = Quaternion::MakeFromAxisOfRotationAndAngleDegrees( worldUp, segmentStartAngle );
	stepStartVector				  = rotorForStartPoint * stepStartVector;

	Quaternion rotorForSegmentStep = Quaternion::MakeFromAxisOfRotationAndAngleDegrees( worldUp, stepAngle );
	Vec3	   stepEndVector	   = stepStartVector;
	stepEndVector				   = rotorForSegmentStep * stepEndVector;
	AddVertsForCylinder3D( localVerts, stepStartVector, stepEndVector, ringThickness, color );

	// reset of segments
	while ( segmentStartAngle < ( sectorEndAngle - stepAngle ) )
	{
		segmentStartAngle += stepAngle;

		stepStartVector = rotorForSegmentStep * stepStartVector;
		stepEndVector	= rotorForSegmentStep * stepEndVector;
		AddVertsForCylinder3D( localVerts, stepStartVector, stepEndVector, ringThickness, color );
	}

	Vec3 coneStart = stepEndVector;
	Vec3 coneEnd   = rotorForSegmentStep * stepEndVector;

	// arrow
	AddVertsForCone3D( localVerts, coneStart, coneEnd, arrowRadius, color );


	// rotate ring to the correct orientation
	Vec3 perpendicularPlane;

	float epsilon	  = 0.00001f;
	/*float xComparison = ( planeNormal.x > -epsilon && planeNormal.x < epsilon );
	float yComparison = ( planeNormal.y > -epsilon && planeNormal.y < epsilon );
	float zComparison = ( ( 1.f - planeNormal.z ) > -epsilon && ( 1.f - planeNormal.z ) < epsilon );*/

	if ( ( planeNormal.x > -epsilon && planeNormal.x < epsilon ) &&
		 ( planeNormal.y > -epsilon && planeNormal.y < epsilon ) &&
		 ( ( 1.f - fabsf( planeNormal.z ) ) > -epsilon && ( 1.f - fabsf( planeNormal.z ) ) < epsilon ) )
	{
		Vec3 worldNorth	   = Vec3( 1.f, 0.f, 0.f );
		perpendicularPlane = CrossProduct3D( worldNorth, planeNormal );
	}
	else
	{
		perpendicularPlane = CrossProduct3D( worldUp, planeNormal );
	}
	float	   angleBetweenQuaternions = GetAngleDegreesBetweenVectors3D( worldUp, planeNormal );
	Quaternion finalRotor			   = Quaternion::MakeFromAxisOfRotationAndAngleDegrees( perpendicularPlane, angleBetweenQuaternions );
	for ( int index = 0; index < localVerts.size(); index++ )
	{
		Vec3& position = localVerts[ index ].m_position;
		position	   = finalRotor * position;
		position += sectorCenter;
	}

	verts.insert( verts.end(), localVerts.begin(), localVerts.end() );
}