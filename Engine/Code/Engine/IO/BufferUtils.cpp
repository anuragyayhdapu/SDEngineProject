#include "BufferUtils.hpp"


#include <cstdint>


//----------------------------------------------------------------------------------------------------------
eBufferEndian GetPlatformEndianness()
{
	// Hex:							0xABCD
	// Binary:						0b1010'1011'1100'1101
	// In Memory (big-endian):		AB CD /or/ 1010 1011 1100 1101  (most significant byte first)
	// In Memory (little-endian):	CD AB /or/ 1100 1101 1010 1011  (least significant byte first)

	uint16_t value	   = 0xABCD; // 0b1010'1011'1100'1101
	uint8_t* firstByte = reinterpret_cast<uint8_t*>( &value );

	if (*firstByte == 0xCD) // 0b0000'0000'1100'1101
	{
		return eBufferEndian::LITTLE_ENDIAN;
	}
	else
	{
		return eBufferEndian::BIG_ENDIAN;
	}
}


//----------------------------------------------------------------------------------------------------------
void Reverse2BytesInPlace( void* bytesToReverseStartAddr )
{
	uint16_t originalWordData			   = *reinterpret_cast<uint16_t*>( bytesToReverseStartAddr );
	*( uint16_t* ) bytesToReverseStartAddr = ( ( originalWordData & 0x00'FF ) << 8 |
											   ( originalWordData & 0xFF'00 ) >> 8 );
}


//----------------------------------------------------------------------------------------------------------
void Reverse4BytesInPlace( void* bytesToReverseStartAddr )
{
	uint32_t originalDWordData			   = *reinterpret_cast<uint32_t*>( bytesToReverseStartAddr );
	*( uint32_t* ) bytesToReverseStartAddr = ( ( ( originalDWordData & 0x00'00'00'FF ) << 24 ) |
											   ( ( originalDWordData & 0x00'00'FF'00 ) << 8 ) |
											   ( ( originalDWordData & 0x00'FF'00'00 ) >> 8 ) |
											   ( ( originalDWordData & 0xFF'00'00'00 ) >> 24 ) );
}


//----------------------------------------------------------------------------------------------------------
void Reverse8BytesInPlace( void* bytesToReverseStartAddr )
{
	uint64_t originalDWordData			   = *reinterpret_cast<uint64_t*>( bytesToReverseStartAddr );
	*( uint64_t* ) bytesToReverseStartAddr = ( ( ( originalDWordData & 0x00'00'00'00'00'00'00'FF ) << 56 ) |
											   ( ( originalDWordData & 0x00'00'00'00'00'00'FF'00 ) << 40 ) |
											   ( ( originalDWordData & 0x00'00'00'00'00'FF'00'00 ) << 24 ) |
											   ( ( originalDWordData & 0x00'00'00'00'FF'00'00'00 ) << 8 ) |
											   ( ( originalDWordData & 0x00'00'00'FF'00'00'00'00 ) >> 8 ) |
											   ( ( originalDWordData & 0x00'00'FF'00'00'00'00'00 ) >> 24 ) |
											   ( ( originalDWordData & 0x00'FF'00'00'00'00'00'00 ) >> 40 ) |
											   ( ( originalDWordData & 0xFF'00'00'00'00'00'00'00 ) >> 56 ) );
}