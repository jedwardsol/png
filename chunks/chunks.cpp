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
    }


    return 0;
}
catch(std::exception const &e)
{
    print("main : {}\n",e.what());
}