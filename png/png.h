#pragma once

#include <cstdint>
#include <vector>
#include <span>
#include <string_view>
#include <filesystem>

namespace fs=std::filesystem;

using ByteSpan = std::span<std::byte>;


namespace pngUtility
{

std::vector<std::byte> readFileBytes(fs::path  const &filename);

uint32_t               crc32(ByteSpan  bytes);


}


namespace png
{


struct Chunk
{
    std::string_view        name;
    ByteSpan                data;
};

std::vector<Chunk> chunks(ByteSpan bytes);


}


#pragma comment(lib,"png")