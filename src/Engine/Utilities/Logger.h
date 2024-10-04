//
// Created by Simeon on 9/29/2024.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <string>
#include <mutex>
#include <imgui.h>

class Logger {
public:
    void Log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);
        logBuffer.push_back(message);
    }

    void Display() {
        std::lock_guard<std::mutex> lock(mutex);
        if (ImGui::Begin("Console", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing)) {
            for (const auto& message : logBuffer) {
                ImGui::Text("%s", message.c_str());
            }
            if (ImGui::Button("Clear Log")) {
                Clear();
            }
        }
        ImGui::End();
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex);
        logBuffer.clear();
    }

private:
    std::vector<std::string> logBuffer;
    std::mutex mutex;
};

inline Logger logger;


#endif //LOGGER_H
