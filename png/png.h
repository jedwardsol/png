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



enum class ColourType
{
    greyscale       = 0,
    rgb             = 2,
    palette         = 3,
    greyscaleAlpha  = 4,
    rgbAlpha        = 6,
};


enum class Compression
{
    deflate,
};

enum class Filter
{
    adaptive,
};

enum class Interlacing
{
    none,
    Adam7
};




struct Header
{
    int                     width;
    int                     height;
    int                     bitDepth;
    ColourType              colourType;
    Compression             compression;
    Filter                  filter;
    Interlacing             interlacing;
};


std::vector<Chunk> chunks(ByteSpan bytes);


Header      header(Chunk                const &chunk);
Header      header(std::vector<Chunk>   const &chunks);


}


#pragma comment(lib,"png")