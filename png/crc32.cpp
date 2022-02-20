#include "png.h"

#include <cstdint>
#include <fstream>
#include <vector>
#include <format>
#include <array>

#include "include/thrower.h"



namespace pngUtility
{

namespace
{

consteval uint32_t reverse(uint32_t value)
{
	uint32_t	result{};

	for(int bit=0;bit<32;bit++)
	{
		result <<= 1;

		result |= value & 1;

		value >>= 1;
	}

	return result;
}

consteval auto calculateTable()
{
	uint32_t const			 polynomial{0x04C11DB7};
	std::array<uint32_t,256> table     {};

	for (int i = 0; i < 256; i++)
	{
		table[i] = reverse(i);

		for (int bit = 0; bit < 8; bit++)
		{
			table[i] =   (table[i] << 1)
		               ^ ((table[i] & (1 << 31)) ? polynomial : 0);
		}

		table[i] = reverse(table[i]);
	}

	return table;
}

}//namespace


uint32_t               crc32(ByteSpan  bytes)
{
	auto static constexpr table = calculateTable();

	uint32_t  crc32{0xffffffff};

	for(auto const byte : bytes)
	{
		auto index = (crc32 & 0xff) ^ static_cast<uint8_t>(byte);
		crc32      = (crc32 >> 8)   ^ table[index];
	}

	return crc32 ^  0xffffffff; 
}

} //namespace pngUtility
