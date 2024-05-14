#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/IO/BufferWriter.hpp"
#include <string>

//----------------------------------------------------------------------------------------------------------
BufferWriter::BufferWriter( Buffer& buffer, eBufferEndian endianMode ) 
	: m_buffer( buffer ), m_currentEndianness( endianMode )
{
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::SetEndianMode( eBufferEndian endianness )
{
	eBufferEndian nativeEndianness = GetPlatformEndianness();
	if ( endianness == eBufferEndian::NATIVE_ENDIAN )
	{
		return;
	}
	m_currentEndianness		 = endianness;
	m_isOppositeNativeEndian = ( nativeEndianness != endianness );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendByte( uint8_t _byte )
{
	m_buffer.push_back( _byte );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendChar( int8_t _char )
{
	m_buffer.push_back( ( uint8_t ) _char );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendBool( bool _bool )
{
	m_buffer.push_back( ( uint8_t ) _bool );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendUnsignedShort( uint16_t _ushort )
{
	uint8_t lowerByte = _ushort & 0xFF;
	uint8_t higerByte = ( _ushort >> 8 ) & 0xFF;

	if ( m_currentEndianness == BIG_ENDIAN )
	{
		m_buffer.push_back( higerByte );
		m_buffer.push_back( lowerByte );
	}
	else if ( m_currentEndianness == LITTLE_ENDIAN )
	{
		m_buffer.push_back( lowerByte );
		m_buffer.push_back( higerByte );
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendShort( int16_t _short )
{
	uint8_t lowerByte = _short & 0xFF;
	uint8_t higerByte = ( _short >> 8 ) & 0xFF;

	if ( m_currentEndianness == BIG_ENDIAN )
	{
		m_buffer.push_back( higerByte );
		m_buffer.push_back( lowerByte );
	}
	else if ( m_currentEndianness == LITTLE_ENDIAN )
	{
		m_buffer.push_back( lowerByte );
		m_buffer.push_back( higerByte );
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendUnsignedInt( uint32_t _uint )
{
	// lower bytes
	uint8_t byte1 = _uint & 0xFF;
	uint8_t byte2 = ( _uint >> 8 ) & 0xFF;

	// higher bytes
	uint8_t byte3 = ( _uint >> 16 ) & 0xFF;
	uint8_t byte4 = ( _uint >> 24 ) & 0xFF;

	if ( m_currentEndianness == BIG_ENDIAN )
	{
		// higher bits first
		m_buffer.push_back( byte4 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte1 );
	}
	else if ( m_currentEndianness == LITTLE_ENDIAN )
	{
		// lower bits first
		m_buffer.push_back( byte1 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte4 );
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendInt( int32_t _int )
{
	// lower bytes
	uint8_t byte1 = _int & 0xFF;
	uint8_t byte2 = ( _int >> 8 ) & 0xFF;

	// higher bytes
	uint8_t byte3 = ( _int >> 16 ) & 0xFF;
	uint8_t byte4 = ( _int >> 24 ) & 0xFF;

	if ( m_currentEndianness == BIG_ENDIAN )
	{
		// higher bits first
		m_buffer.push_back( byte4 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte1 );
	}
	else if ( m_currentEndianness == LITTLE_ENDIAN )
	{
		// lower bits first
		m_buffer.push_back( byte1 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte4 );
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendUnsignedInt64Bit( uint64_t _uint64 )
{
	// lower bytes
	uint8_t byte1 = _uint64 & 0xFF;
	uint8_t byte2 = ( _uint64 >> 8 ) & 0xFF;
	uint8_t byte3 = ( _uint64 >> 16 ) & 0xFF;
	uint8_t byte4 = ( _uint64 >> 24 ) & 0xFF;

	// higher bytes
	uint8_t byte5 = ( _uint64 >> 32 ) & 0xFF;
	uint8_t byte6 = ( _uint64 >> 40 ) & 0xFF;
	uint8_t byte7 = ( _uint64 >> 48 ) & 0xFF;
	uint8_t byte8 = ( _uint64 >> 56 ) & 0xFF;

	if ( m_currentEndianness == BIG_ENDIAN )
	{
		// higher bits first
		m_buffer.push_back( byte8 );
		m_buffer.push_back( byte7 );
		m_buffer.push_back( byte6 );
		m_buffer.push_back( byte5 );
		m_buffer.push_back( byte4 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte1 );
	}
	else if ( m_currentEndianness == LITTLE_ENDIAN )
	{
		// lower bits first
		m_buffer.push_back( byte1 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte4 );
		m_buffer.push_back( byte5 );
		m_buffer.push_back( byte6 );
		m_buffer.push_back( byte7 );
		m_buffer.push_back( byte8 );
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendInt64Bit( int64_t _int64Bit )
{
	// lower bytes
	uint8_t byte1 = _int64Bit & 0xFF;
	uint8_t byte2 = ( _int64Bit >> 8 ) & 0xFF;
	uint8_t byte3 = ( _int64Bit >> 16 ) & 0xFF;
	uint8_t byte4 = ( _int64Bit >> 24 ) & 0xFF;

	// higher bytes
	uint8_t byte5 = ( _int64Bit >> 32 ) & 0xFF;
	uint8_t byte6 = ( _int64Bit >> 40 ) & 0xFF;
	uint8_t byte7 = ( _int64Bit >> 48 ) & 0xFF;
	uint8_t byte8 = ( _int64Bit >> 56 ) & 0xFF;

	if ( m_currentEndianness == BIG_ENDIAN )
	{
		// higher bits first
		m_buffer.push_back( byte8 );
		m_buffer.push_back( byte7 );
		m_buffer.push_back( byte6 );
		m_buffer.push_back( byte5 );
		m_buffer.push_back( byte4 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte1 );
	}
	else if ( m_currentEndianness == LITTLE_ENDIAN )
	{
		// lower bits first
		m_buffer.push_back( byte1 );
		m_buffer.push_back( byte2 );
		m_buffer.push_back( byte3 );
		m_buffer.push_back( byte4 );
		m_buffer.push_back( byte5 );
		m_buffer.push_back( byte6 );
		m_buffer.push_back( byte7 );
		m_buffer.push_back( byte8 );
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendFloat( float _float )
{
	// 32 bit float
	float*	  floatMemAddr	= &_float;
	uint32_t* uint32MemAddr = reinterpret_cast<uint32_t*>( floatMemAddr );
	AppendUnsignedInt( *uint32MemAddr );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendDouble( double _double )
{
	// 64 bit double
	double*	  doubleMemAddr = &_double;
	uint64_t* uint64MemAddr = reinterpret_cast<uint64_t*>( doubleMemAddr );
	AppendUnsignedInt64Bit( *uint64MemAddr );

}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendStringZeroTerminated( char const* _zero )
{
	size_t charIndex = 0;
	while ( true )
	{
		char charToAppend = _zero[ charIndex++ ];
		AppendChar( charToAppend );
		if ( charToAppend == '\0' )
		{
			return;
		}
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendStringAfter32BitLength( char const* stringToAppend )
{
	size_t bufferIndexBeforeAppendingString = m_buffer.size() - 1;
	AppendUnsignedInt( 0 );
	uint32_t charIndex = 0;
	while ( true )
	{
		char charToAppend = stringToAppend[ charIndex++ ];
		if ( charToAppend == '\0' )
		{

			uint64_t writePosOffset = bufferIndexBeforeAppendingString + 1;
			uint32_t overwrittingDWord = --charIndex;

			GUARANTEE_OR_DIE( ( m_buffer.size() > writePosOffset ), "Write Position Offset out of bounds" );
			uint8_t*  bufferStartAddress = m_buffer.data();
			uint32_t* bufferWriteAddress = reinterpret_cast<uint32_t*>( bufferStartAddress + writePosOffset );
			*bufferWriteAddress			 = overwrittingDWord;
			if ( m_isOppositeNativeEndian )
			{
				Reverse4BytesInPlace( bufferWriteAddress );
			}

			return;
		}
		AppendChar( charToAppend );
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendRgba( Rgba8 rgbaToAppend )
{
	AppendByte( rgbaToAppend.r );
	AppendByte( rgbaToAppend.g );
	AppendByte( rgbaToAppend.b );
	AppendByte( rgbaToAppend.a );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendRgb( Rgba8 rgbToAppend )
{
	AppendByte( rgbToAppend.r );
	AppendByte( rgbToAppend.g );
	AppendByte( rgbToAppend.b );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendIntVec2( IntVec2 intVec2ToAppend )
{
	AppendInt( intVec2ToAppend.x );
	AppendInt( intVec2ToAppend.y );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendVec2( Vec2 vec2ToAppend )
{
	AppendFloat( vec2ToAppend.x );
	AppendFloat( vec2ToAppend.y );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendVec3( Vec3 vec3ToAppend )
{
	AppendFloat( vec3ToAppend.x );
	AppendFloat( vec3ToAppend.y );
	AppendFloat( vec3ToAppend.z );
}


//----------------------------------------------------------------------------------------------------------
void BufferWriter::AppendVertexPCU( Vertex_PCU const& vertexToAppend )
{
	AppendVec3( vertexToAppend.m_position );
	AppendRgba( vertexToAppend.m_color );
	AppendVec2( vertexToAppend.m_uvTexCoords );
}
