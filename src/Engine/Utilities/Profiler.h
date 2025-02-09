//
// Created by Simeon on 10/11/2024.
//

#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>
#include <string>

#define PROFILE_SCOPE(name) Profiler _profiler_##__LINE__(name);

struct Profiler {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;

    Profiler(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
    ~Profiler() {
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "[PROFILE] " << name << " took " << duration << " ms" << std::endl;
    }
};

#endif //PROFILER_H
