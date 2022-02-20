#include "png.h"

#include <cassert>
#include <cstdint>
#include <fstream>
#include <vector>
#include <format>
#include <array>


#include "include/thrower.h"


using namespace std::literals;

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
void consumeFileHeader(ByteSpan &bytes)
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

uint8_t    consumeUint8(ByteSpan &bytes)
{
    auto value = static_cast<uint8_t>(bytes.front());

    bytes = bytes.subspan(1);
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



    auto crc32data = bytes.subspan(0, 4+size);

    bytes=bytes.subspan(crc32data.size());

    auto const expectedCrc32     = consumeUint32(bytes);
    auto const actualCrc32       = pngUtility::crc32(crc32data);


    if(expectedCrc32 != actualCrc32)
    {
        throw_runtime_error(std::format("Chunk checksum doesn't match {:08x}!={:08x}",actualCrc32, expectedCrc32));
    }

    return chunk;
}

}


std::vector<Chunk> chunks(std::span<std::byte>  bytes)
{
    consumeFileHeader(bytes);

    std::vector<Chunk>  chunks;

    while(bytes.size())
    {
        chunks.push_back( consumeChunk(bytes));
    }

    if(chunks.size() < 2)
    {
        throw_runtime_error(std::format("Expected at least 2 chunks : only found {}\n",chunks.size()));
    }

    if(chunks.front().name != "IHDR"sv)
    {
        throw_runtime_error(std::format("First chunk should be IHDR but is {}\n",chunks.front().name));
    }

    if(chunks.back().name != "IEND"sv)
    {
        throw_runtime_error(std::format("Last chunk should be IEND but is {}\n",chunks.back().name));
    }

    return chunks;
}


Header      header(Chunk                const &chunk)
{
    assert(chunk.name        == "IHDR"sv);

    if(chunk.data.size() != 13)
    {
        throw_runtime_error("IHDR chunk isn't 13 bytes");
    }

    auto data = chunk.data;

    Header header
    {
        static_cast<int>(consumeUint32(data)),
        static_cast<int>(consumeUint32(data)),
        static_cast<int>(consumeUint8(data)),

        static_cast<ColourType >(consumeUint8(data)),
        static_cast<Compression>(consumeUint8(data)),
        static_cast<Filter     >(consumeUint8(data)),
        static_cast<Interlacing>(consumeUint8(data)),

    };


    return header;
}


}