#pragma once

#include <vector>


//----------------------------------------------------------------------------------------------------------
typedef std::vector<unsigned char> Buffer;


//----------------------------------------------------------------------------------------------------------
enum eBufferEndian : unsigned char
{
	NATIVE_ENDIAN,
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};


eBufferEndian GetPlatformEndianness();
void		  Reverse2BytesInPlace( void* bytesToReverseStartAddr );
void		  Reverse4BytesInPlace( void* bytesToReverseStartAddr );
void		  Reverse8BytesInPlace( void* bytesToReverseStartAddr );