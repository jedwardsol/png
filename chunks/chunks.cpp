#include <cstdint>
#include <fstream>
#include <vector>
#include <string>
#include <exception>

using namespace std::literals;
#include "include/thrower.h"
#include "include/print.h"

#include "png/png.h"



int main(int argc, const char *argv[])
try
{
    std::vector<std::string> const args(argv+1,argv+argc);

    if(args.empty())
    {
        throw_runtime_error("filename\n");
    }

    auto bytes  = pngUtility::readFileBytes(args[0]);
    auto chunks = png::chunks(bytes);

    for(auto const &chunk : chunks)
    {
        print("Chunk {} : {:8} bytes\n", chunk.name, chunk.data.size());

        if(chunk.name == "IHDR"sv)
        {
            auto header = png::header(chunk);
        
            print("    width       {} \n", header.width);
            print("    height      {} \n", header.height);
            print("    bitDepth    {} \n", header.bitDepth);
            print("    colourType  {} \n", static_cast<int>(header.colourType));
            print("    compression {} \n", static_cast<int>(header.compression));
            print("    filter      {} \n", static_cast<int>(header.filter));
            print("    interlacing {} \n", static_cast<int>(header.interlacing));
        }
    }

    return 0;
}
catch(std::exception const &e)
{
    print("main : {}\n",e.what());
}