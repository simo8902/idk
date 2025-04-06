//
// Created by simeon on 31.01.25.
//

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H


#include "Shader.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include <mutex>

class ShaderManager {
public:
    static ShaderManager& Instance() {
        static ShaderManager instance;
        return instance;
    }

    void Initialize();
    void Shutdown();
    void ScanDirectory(const std::filesystem::path& directory = "shaders");
    void ReloadAll();
    void HandleFileDrop(const std::vector<std::string>& paths);

    std::shared_ptr<IDK::Graphics::Shader> GetShader(const std::string& name);
    const auto& getShaders() const { return shaders; }
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    std::shared_ptr<IDK::Graphics::Shader> getShaderProgram() const { return shaderProgram; }
    std::shared_ptr<IDK::Graphics::Shader> getLightShader() const { return lightShader; }
    std::shared_ptr<IDK::Graphics::Shader> getFinalPassShader() const { return finalPassShader; }
    std::shared_ptr<IDK::Graphics::Shader> getSkyShader() const { return skyShader; }

private:
    ShaderManager();
    ~ShaderManager();

    std::shared_ptr<IDK::Graphics::Shader> shaderProgram;
    std::shared_ptr<IDK::Graphics::Shader> lightShader;
    std::shared_ptr<IDK::Graphics::Shader> finalPassShader;
    std::shared_ptr<IDK::Graphics::Shader> skyShader;


    std::unordered_map<std::string, std::shared_ptr<IDK::Graphics::Shader>> shaders;
    std::vector<std::filesystem::path> searchPaths;
    std::unique_ptr<std::thread> fileWatcher;
    std::atomic<bool> running{false};
    std::mutex shaderMutex;
    std::unordered_map<std::string, std::filesystem::file_time_type> fileTimestamps;

    void FileWatchLoop();
    void UpdateFileMonitoring();
    void LoadShader(const std::filesystem::path& path);
    void ReloadShader(const std::string& name);
    std::string readFile(const std::string& filePath);
};


#endif //SHADERMANAGER_H