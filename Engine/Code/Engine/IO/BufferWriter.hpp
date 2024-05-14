#pragma once

#include "Engine/IO/BufferUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"

#include <vector>


//----------------------------------------------------------------------------------------------------------
class BufferWriter
{
public:
	BufferWriter( Buffer& buffer, eBufferEndian endianMode = eBufferEndian::NATIVE_ENDIAN );

	//----------------------------------------------------------------------------------------------------------
	void SetEndianMode(eBufferEndian endianness);
	eBufferEndian GetEndianMode() { return m_currentEndianness; }

	//----------------------------------------------------------------------------------------------------------
	void AppendByte( uint8_t _byte );
	void AppendChar( int8_t _char );
	void AppendBool( bool _bool );
	void AppendUnsignedShort( uint16_t _ushort );
	void AppendShort( int16_t _short );
	void AppendUnsignedInt( uint32_t _uint );
	void AppendInt( int32_t _int );
	void AppendUnsignedInt64Bit( uint64_t _uint64 );
	void AppendInt64Bit( int64_t _int64Bit );
	void AppendFloat( float _float );
	void AppendDouble( double _double );
	void AppendStringZeroTerminated( char const* _zero );
	void AppendStringAfter32BitLength( char const* stringToAppend );
	void AppendRgba( Rgba8 rgbaToAppend );
	void AppendRgb( Rgba8 rgbToAppend );
	void AppendIntVec2( IntVec2 intVec2ToAppend );
	void AppendVec2( Vec2 vec2ToAppend );
	void AppendVec3( Vec3 vec2ToAppend );
	void AppendVertexPCU( Vertex_PCU const& vertexToAppend );

	//----------------------------------------------------------------------------------------------------------
	std::vector<uint8_t>& m_buffer;
	eBufferEndian		  m_currentEndianness	   = eBufferEndian::NATIVE_ENDIAN;
	bool				  m_isOppositeNativeEndian = false;
};