#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/IO/BufferReader.hpp"


//----------------------------------------------------------------------------------------------------------
BufferReader::BufferReader( unsigned char const* bufferToParse, size_t bufferSizeInBytes, eBufferEndian endianMode ) 
	: m_bufferStart( bufferToParse ),
	m_bufferSize( bufferSizeInBytes )
{
	SetEndianMode( endianMode );
}


//----------------------------------------------------------------------------------------------------------
BufferReader::BufferReader( Buffer const& buffer, eBufferEndian endianMode )
	: m_bufferStart( buffer.data() ),
	  m_bufferSize( buffer.size() )
{
	SetEndianMode( endianMode );
}


//----------------------------------------------------------------------------------------------------------
void BufferReader::SetEndianMode( eBufferEndian endianMode )
{
	eBufferEndian nativeEndianness = GetPlatformEndianness();
	if ( endianMode == eBufferEndian::NATIVE_ENDIAN )
	{
		return;
	}
	m_currentEndianness			= endianMode;
	m_isOppositeNativeEndianess = ( nativeEndianness != endianMode );
}


//----------------------------------------------------------------------------------------------------------
unsigned char BufferReader::ParseByte()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 1 <= m_bufferSize, "Parsing Index out of bounds" );
	return m_bufferStart[ m_currentOffsetFromStart++ ];
}


//----------------------------------------------------------------------------------------------------------
char BufferReader::ParseChar()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 1 <= m_bufferSize, "Parsing Index out of bounds" );
	return ( char ) ParseByte();
}


//----------------------------------------------------------------------------------------------------------
bool BufferReader::ParseBool()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 1 <= m_bufferSize, "Parsing Index out of bounds" );
	return m_bufferStart[ m_currentOffsetFromStart++ ];
}


//----------------------------------------------------------------------------------------------------------
uint16_t BufferReader::ParseUShort16()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 2 <= m_bufferSize, "Parsing Index out of bounds" );
	uint16_t const* memAddr			  = ( uint16_t* ) ( &m_bufferStart[ m_currentOffsetFromStart ] );
	uint16_t		valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse2BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 2;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
int16_t BufferReader::ParseShort16()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 2 <= m_bufferSize, "Parsing Index out of bounds" );
	int16_t const* memAddr			 = ( int16_t* ) ( &m_bufferStart[ m_currentOffsetFromStart ] );
	int16_t		   valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse2BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 2;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
uint32_t BufferReader::ParseUint32()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 4 <= m_bufferSize, "Parsing Index out of bounds" );
	uint32_t const* memAddr			  = ( uint32_t* ) ( &m_bufferStart[ m_currentOffsetFromStart ] );
	uint32_t		valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse4BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 4;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
int32_t BufferReader::ParseInt32()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 4 <= m_bufferSize, "Parsing Index out of bounds" );
	int32_t const* memAddr			 = ( int32_t* ) ( &m_bufferStart[ m_currentOffsetFromStart ] );
	int32_t		   valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse4BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 4;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
uint64_t BufferReader::ParseUInt64()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 8 <= m_bufferSize, "Parsing Index out of bounds" );
	uint64_t const* memAddr			  = ( uint64_t* ) ( &m_bufferStart[ m_currentOffsetFromStart ] );
	uint64_t		valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse8BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 8;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
int64_t BufferReader::ParseInt64()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 8 <= m_bufferSize, "Parsing Index out of bounds" );
	int64_t const* memAddr			 = ( int64_t* ) ( &m_bufferStart[ m_currentOffsetFromStart ] );
	int64_t		   valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse8BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 8;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
float BufferReader::ParseFloat()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 4 <= m_bufferSize, "Parsing Index out of bounds" );
	float const* memAddr		   = ( float* ) ( &m_bufferStart[ m_currentOffsetFromStart ] );
	float		 valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse4BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 4;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
double BufferReader::ParseDouble()
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 8 <= m_bufferSize, "Parsing Index out of bounds" );
	double const* memAddr			= reinterpret_cast<double const*>( &m_bufferStart[ m_currentOffsetFromStart ] );
	double		  valAtGivenAddress = *memAddr;
	if ( m_isOppositeNativeEndianess )
	{
		Reverse8BytesInPlace( &valAtGivenAddress );
	}
	m_currentOffsetFromStart += 8;
	return valAtGivenAddress;
}


//----------------------------------------------------------------------------------------------------------
void BufferReader::ParseStringZeroTerminated( std::string& parsedZeroTerminatedString )
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 1 <= m_bufferSize, "Parsing Index out of bounds" );
	while ( true )
	{
		char parsedChar = ParseChar();
		if ( parsedChar == '\0' )
		{
			return;
		}
		parsedZeroTerminatedString += parsedChar;
	}
}


//----------------------------------------------------------------------------------------------------------
void BufferReader::ParseStringAfter32BitLength( std::string& parsedZeroTerminatedString )
{
	GUARANTEE_OR_DIE( m_currentOffsetFromStart + 1 <= m_bufferSize, "Parsing Index out of bounds" );
	size_t	 stringIndex  = parsedZeroTerminatedString.size();
	uint32_t stringLength = ParseUint32();
	parsedZeroTerminatedString.reserve( stringLength + stringIndex );
	while ( stringLength-- )
	{
		char parsedChar	 = ParseChar();
		parsedZeroTerminatedString += parsedChar;
	}
}


//----------------------------------------------------------------------------------------------------------
Vec2 BufferReader::ParseVec2()
{
	Vec2 parsedVec2;
	parsedVec2.x = ParseFloat();
	parsedVec2.y = ParseFloat();
	return parsedVec2;
}


//----------------------------------------------------------------------------------------------------------
Vec3 BufferReader::ParseVec3()
{
	Vec3 parsedVec3;
	parsedVec3.x = ParseFloat();
	parsedVec3.y = ParseFloat();
	parsedVec3.z = ParseFloat();
	return parsedVec3;
}


//----------------------------------------------------------------------------------------------------------
IntVec2 BufferReader::ParseIntVec2()
{
	IntVec2 parsedIntVec;
	parsedIntVec.x = ParseInt32();
	parsedIntVec.y = ParseInt32();
	return parsedIntVec;
}


//----------------------------------------------------------------------------------------------------------
Rgba8 BufferReader::ParseRgba()
{
	Rgba8 parsedColor;
	parsedColor.r = ParseChar();
	parsedColor.g = ParseChar();
	parsedColor.b = ParseChar();
	parsedColor.a = ParseChar();
	return parsedColor;
}


//----------------------------------------------------------------------------------------------------------
Rgba8 BufferReader::ParseRgb()
{
	Rgba8 parsedColor;
	parsedColor.r = ParseChar();
	parsedColor.g = ParseChar();
	parsedColor.b = ParseChar();
	return parsedColor;
}


//----------------------------------------------------------------------------------------------------------
Vertex_PCU BufferReader::ParseVertexPCU()
{
	Vertex_PCU parsedVertex;
	parsedVertex.m_position	   = ParseVec3();
	parsedVertex.m_color	   = ParseRgba();
	parsedVertex.m_uvTexCoords = ParseVec2();
	return parsedVertex;
}
