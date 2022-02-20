#include "png.h"

#include <cstdint>
#include <fstream>
#include <vector>
#include <format>
#include <array>

#include "include/thrower.h"

namespace pngUtility
{

std::vector<std::byte> readFileBytes(fs::path  const &filename)
{
    auto size=fs::file_size(filename);

    if(size==0)
    {
        throw_runtime_error(std::format("File {} is empty",filename.string()));
    }

    std::ifstream   file{filename, std::ios::binary};
    
    if(!file)
    {
        throw_system_error(std::format("Failed to open {}",filename.string()));
    }

    std::vector<std::byte>  bytes(size);

    file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

    if(!file)
    {
        throw_system_error(std::format("Failed to read {}",filename.string()));
    }


    return bytes;
}

}




namespace png
{

namespace 
{
void consumeHeader(ByteSpan &bytes)
{
    static constexpr std::array<uint8_t,8> header
    {{
        0x89,
        0x50, 0x4E, 0x47,
        0x0D, 0x0A,
        0x1a, 0x0a
    }};

    if(bytes.size() < header.size())
    {
        throw_runtime_error(std::format("Data isn't long enough ({} bytes) for header",bytes.size()));
    }

    if(memcmp(bytes.data(), header.data(), header.size())!=0)
    {
        throw_runtime_error("Data doesn't start with PNG header");
    }

    bytes = bytes.subspan(header.size());
}


uint32_t    consumeUint32(ByteSpan &bytes)
{
    uint32_t    value{};

    memcpy(&value,bytes.data(),4);

    value=_byteswap_ulong(value);


    bytes = bytes.subspan(4);
    return value;
}

Chunk consumeChunk(ByteSpan &bytes)
{
    if(bytes.size() < 3 * sizeof(uint32_t))
    {
        throw_runtime_error(std::format("Data isn't long enough ({} bytes) for another chunk",bytes.size()));
    }

    auto const size=consumeUint32(bytes);

    if(size > bytes.size())
    {
        throw_runtime_error("Chunk size is larger than remaining data");
    }


    Chunk   chunk{};

    chunk.name      = std::string_view{reinterpret_cast<char const*>(bytes.data()),4};
    chunk.data      = bytes.subspan(4, size);
    chunk.crc32data = bytes.subspan(0, 4+size);

    bytes=bytes.subspan(chunk.crc32data.size());

    chunk.crc32 = consumeUint32(bytes);


    return chunk;
}

}


std::vector<Chunk> chunks(std::span<std::byte>  bytes)
{
    consumeHeader(bytes);

    std::vector<Chunk>  chunks;

    while(bytes.size())
    {
        chunks.push_back( consumeChunk(bytes));
    }


    return chunks;
}



}