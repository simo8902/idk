//
// Created by Simeon on 10/11/2024.
//

#ifndef PROFILER_H
#define PROFILER_H
#include <chrono>
#include <string>
#include <unordered_map>

#include "imgui.h"

class Profiler {
public:
    struct ResourceUsage {
        double cpuTime = 0.0;  // CPU time in seconds
    };

    // Start tracking a component's CPU usage
    void StartTracking(const std::string& componentName) {
        startTimes[componentName] = std::chrono::high_resolution_clock::now();
    }

    // Stop tracking and accumulate the CPU time
    void StopTracking(const std::string& componentName) {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTimes[componentName];
        resourceUsages[componentName].cpuTime += elapsed.count();
    }

    // Record total runtime to calculate percentages later
    void UpdateTotalRuntime(double frameTime) {
        totalRuntime += frameTime;
    }

    // Render the profiler using ImGui
    void RenderImGui() {
        ImGui::Begin("Profiler");

        for (const auto& [componentName, usage] : resourceUsages) {
            double percentage = (totalRuntime > 0) ? (usage.cpuTime / totalRuntime) * 100.0 : 0.0;
            ImGui::Text("Component: %s", componentName.c_str());
            ImGui::Text("CPU Time: %.6f seconds (%.2f%% of runtime)", usage.cpuTime, percentage);
            ImGui::Separator();
        }

        ImGui::Text("Total Runtime: %.6f seconds", totalRuntime);
        ImGui::End();
    }

private:
    std::unordered_map<std::string, ResourceUsage> resourceUsages;  // Track CPU time for each component
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes;  // Store start times
    double totalRuntime = 0.0;

};

#endif //PROFILER_H
