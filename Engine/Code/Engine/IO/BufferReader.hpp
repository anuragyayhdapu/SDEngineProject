#pragma once

#include "Engine/IO/BufferUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"


#include <string>


//----------------------------------------------------------------------------------------------------------
class BufferReader
{
public:
	BufferReader( unsigned char const* bufferToParse, size_t bufferSizeInBytes, eBufferEndian endianMode = eBufferEndian::NATIVE_ENDIAN );
	BufferReader( Buffer const& buffer, eBufferEndian endianMode = eBufferEndian::NATIVE_ENDIAN );

	void		  SetEndianMode( eBufferEndian endianMode );

	unsigned char ParseByte();
	char		  ParseChar();
	bool		  ParseBool();
	uint16_t	  ParseUShort16();
	int16_t		  ParseShort16();
	uint32_t	  ParseUint32();
	int32_t		  ParseInt32();
	uint64_t	  ParseUInt64();
	int64_t		  ParseInt64();
	float		  ParseFloat();
	double		  ParseDouble();
	void		  ParseStringZeroTerminated( std::string& parsedZeroTerminatedString );
	void		  ParseStringAfter32BitLength( std::string& parsedZeroTerminatedString );
	Vec2		  ParseVec2();
	Vec3		  ParseVec3();
	IntVec2		  ParseIntVec2();
	Rgba8		  ParseRgba();
	Rgba8		  ParseRgb();
	Vertex_PCU	  ParseVertexPCU();

	//----------------------------------------------------------------------------------------------------------
	unsigned char const* m_bufferStart				 = nullptr;
	size_t				 m_bufferSize				 = 0;
	size_t				 m_currentOffsetFromStart	 = 0;
	bool				 m_isOppositeNativeEndianess = false;
	eBufferEndian		 m_currentEndianness		 = eBufferEndian::NATIVE_ENDIAN;
};