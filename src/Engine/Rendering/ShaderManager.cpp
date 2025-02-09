//
// Created by simeon on 31.01.25.
//

#include "ShaderManager.h"

#include <future>

#include "ShaderManager.h"
#include <iostream>
#include <chrono>
#include <fstream>

ShaderManager::ShaderManager() {
    std::filesystem::path resourceShadersPath = SOURCE_DIR "/src/shaders/";
    std::filesystem::path shadersPath = SOURCE_DIR "/ROOT/shaders/";

    /*
    std::cout << "Scanning for shaders in:\n";
    std::cout << "  " << resourceShadersPath << "\n";
    std::cout << "  " << shadersPath << "\n";*/

    searchPaths.emplace_back(resourceShadersPath);
    searchPaths.emplace_back(shadersPath);
}

ShaderManager::~ShaderManager() {
    Shutdown();
}

void ShaderManager::Initialize() {
    running = true;
    fileWatcher = std::make_unique<std::thread>(&ShaderManager::FileWatchLoop, this);
    for (const auto& path : searchPaths) {
        ScanDirectory(path);
    }
}

void ShaderManager::Shutdown() {
    running = false;
    if(fileWatcher && fileWatcher->joinable()) {
        fileWatcher->join();
    }
}

void ShaderManager::FileWatchLoop() {
    while(running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        UpdateFileMonitoring();
    }
}

void ShaderManager::UpdateFileMonitoring() {
    std::lock_guard<std::mutex> lock(shaderMutex);

    for(auto& [name, shader] : shaders) {
        auto checkFile = [&](const std::string& path) {
            if(!path.empty()) {
                auto ftime = std::filesystem::last_write_time(path);
                if(fileTimestamps[path] != ftime) {
                    try {
                        shader->reloadFromPath(path);
                        fileTimestamps[path] = ftime;
                    } catch(const std::exception& e) {
                        std::cerr << "Hot reload failed: " << e.what() << "\n";
                    }
                }
            }
        };

        checkFile(shader->getPaths().vertex);
        checkFile(shader->getPaths().fragment);
    }
}

void ShaderManager::ScanDirectory(const std::filesystem::path& directory) {
     namespace fs = std::filesystem;

  //  std::cout << "Scanning directory: " << fs::absolute(directory) << "\n";

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Shader scan error: Invalid directory: " << fs::absolute(directory) << "\n";
        return;
    }

    try {
        std::unordered_map<std::string, std::pair<std::filesystem::path, std::filesystem::path>> shaderPairs;

        // First pass: collect vertex and fragment shaders
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                const auto& filePath = entry.path();
                const auto& extension = filePath.extension();

                if (extension == ".vert" || extension == ".frag") {
                    std::string name = filePath.stem().string();
                    if (extension == ".vert") {
                        shaderPairs[name].first = filePath; // Store vertex path
                    } else if (extension == ".frag") {
                        shaderPairs[name].second = filePath; // Store fragment path
                    }
                } else if (extension == ".glsl") {
                    // Load combined shader directly
                    LoadShader(filePath);
                }
            }
        }

        // Second pass: create shaders from collected pairs
        for (const auto& [name, paths] : shaderPairs) {
            if (!paths.first.empty() && !paths.second.empty()) {
                // Both vertex and fragment shaders exist
               // std::cout << "Found shader pair: " << name << " (Vertex: " << paths.first << ", Fragment: " << paths.second << ")\n";
                LoadShader(paths.first); // Load vertex shader
                LoadShader(paths.second); // Load fragment shader
            } else {
                if (!paths.first.empty()) {
                    std::cerr << "Warning: Found vertex shader without corresponding fragment: " << paths.first << "\n";
                }
                if (!paths.second.empty()) {
                    std::cerr << "Warning: Found fragment shader without corresponding vertex: " << paths.second << "\n";
                }
            }
        }

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Shader scan error: " << e.what() << "\n";
    }
}
std::string ShaderManager::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

void ShaderManager::LoadShader(const std::filesystem::path& path) {
   try {
        std::string baseName = path.stem().string(); // Base name without extension
        std::lock_guard<std::mutex> lock(shaderMutex);

        // Check if shader is already loaded
        if (shaders.find(baseName) != shaders.end()) {
            ReloadShader(baseName);
            return;
        }

        // Validate the shader file
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
            throw std::runtime_error("Shader file does not exist or is not a regular file: " + path.string());
        }

        auto extension = path.extension().string();
        std::filesystem::path vertPath;  // Declare vertPath
        std::filesystem::path fragPath;  // Declare fragPath

        // Handle separate vertex and fragment shaders
        if (extension == ".frag" || extension == ".vert") {
            // Determine corresponding shader paths
            vertPath = path.parent_path() / (baseName + ".vert");
            fragPath = path.parent_path() / (baseName + ".frag");

            // Ensure both vertex and fragment shaders exist
            if (!std::filesystem::exists(vertPath)) {
                throw std::runtime_error("Vertex shader file does not exist: " + vertPath.string());
            }
            if (!std::filesystem::exists(fragPath)) {
                throw std::runtime_error("Fragment shader file does not exist: " + fragPath.string());
            }

            /*
            std::cout << "Found shader pair: " << baseName
                      << " (Vertex: \"" << vertPath.string()
                      << "\", Fragment: \"" << fragPath.string() << "\")\n";*/

            // Create and compile the combined shader
            auto shader = std::make_shared<Shader>(vertPath.string().c_str(), fragPath.string().c_str());

            if (shader->isValid()) {
                shaders[baseName] = shader;
                fileTimestamps[vertPath.string()] = std::filesystem::last_write_time(vertPath);
                fileTimestamps[fragPath.string()] = std::filesystem::last_write_time(fragPath);
             //   std::cout << "Successfully loaded combined shader: " << baseName << "\n";
            } else {
                throw std::runtime_error("Failed to compile combined shader: " + baseName);
            }
        } else if (extension == ".glsl") {
            auto shader = std::make_shared<Shader>(path.string().c_str(), true);

            if (shader->isValid()) {
                shaders[baseName] = shader;
                fileTimestamps[path.string()] = std::filesystem::last_write_time(path);
              //  std::cout << "Successfully loaded GLSL shader: " << baseName << "\n";
            } else {
                throw std::runtime_error("Failed to compile GLSL shader: " + baseName);
            }

        } else {
            throw std::runtime_error("Unsupported shader file extension: " + extension);
        }
    } catch (const std::exception& e) {
        std::cerr << "Shader load error: " << e.what() << "\n";
    }

}

void ShaderManager::ReloadAll() {
    std::lock_guard<std::mutex> lock(shaderMutex);
    for(auto& [name, _] : shaders) {
        ReloadShader(name);
    }
}

void ShaderManager::ReloadShader(const std::string& name) {
    try {
        auto& shader = shaders.at(name);
        shader->reload();
       // std::cout << "Reloaded shader: " << name << "\n";
    } catch(const std::exception& e) {
        std::cerr << "Shader reload failed: " << name << " - " << e.what() << "\n";
    }
}

void ShaderManager::HandleFileDrop(const std::vector<std::string>& paths) {
    std::lock_guard<std::mutex> lock(shaderMutex);
    for(const auto& path : paths) {
        LoadShader(path);
    }
}

std::shared_ptr<Shader> ShaderManager::GetShader(const std::string& name) {
    std::lock_guard<std::mutex> lock(shaderMutex);
    auto it = shaders.find(name);
    return it != shaders.end() ? it->second : nullptr;
}