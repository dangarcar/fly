#pragma once

#include <chrono>

class Timer {
public:
    Timer() { reset(); }
    void reset() { time = std::chrono::system_clock::now(); }
    float elapsedMillis() { return (std::chrono::system_clock::now() - time).count() / 1e6f; }

private:
    std::chrono::system_clock::time_point time;
};