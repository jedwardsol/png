#pragma once

#include <cstdint>
#include <vector>
#include <span>
#include <string_view>
#include <filesystem>

namespace fs=std::filesystem;


namespace pngUtility
{
std::vector<std::byte> readFileBytes(fs::path  const &filename);
}


namespace png
{

using ByteSpan = std::span<std::byte>;

struct Chunk
{
    std::string_view        name;
    ByteSpan                data;

    ByteSpan                crc32data;
    uint32_t                crc32;    

};

std::vector<Chunk> chunks(ByteSpan bytes);


}


#pragma comment(lib,"png")