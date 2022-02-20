#pragma once
#include <windows.h>
#include <format>
#include <iostream>
#include <vector>

class Console
{
    HANDLE output;
    HANDLE input;
    DWORD  originalMode{};

public:

    Console() : output{GetStdHandle(STD_OUTPUT_HANDLE )} , input{GetStdHandle(STD_INPUT_HANDLE )}
    {
        GetConsoleMode(output,&originalMode),
        SetConsoleMode(output, originalMode|ENABLE_VIRTUAL_TERMINAL_PROCESSING );
    }

    ~Console()
    {
        std::cout << "\x1b[!p";
        SetConsoleMode(output, originalMode);
    }


    void clear()
    {
        std::cout << "\x1b[2J";
    }

    void goTo(int row, int column)
    {
        std::cout << std::format("\x1b[{};{}H",row,column);
    }

    void grey(int percentBrightness)
    {
        auto rgb = 255*percentBrightness/100;

        std::cout <<  std::format("\x1b[38;2;{};{};{}m", rgb,rgb,rgb);
    }

    auto getInput()
    {
        std::vector<INPUT_RECORD>   records(10);

        DWORD read{};

        if(ReadConsoleInput(input,records.data(),static_cast<DWORD>(records.size()),&read))
        {
            records.resize(read);
        }

        

        return records;
    }

};


