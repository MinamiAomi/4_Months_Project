#pragma once
#include <chrono>
#include <functional>

namespace Debug {

    template<typename duration = std::chrono::milliseconds>
    duration ElapsedTime(std::function<void()> func) {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        func();
        return std::chrono::duration_cast<duration>(std::chrono::steady_clock::now() - start);
    }
}