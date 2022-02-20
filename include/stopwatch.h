#pragma once
#include <chrono>

class Stopwatch
{
    using Clock = std::chrono::steady_clock;

public:

    Stopwatch() : start{Clock::now()}
    {}

    void reset()
    {
        start=Clock::now();
    }

    auto seconds()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(Clock::now()-start).count();
    }

    auto milliseconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now()-start).count();
    }

private:

    Clock::time_point       start;
};

