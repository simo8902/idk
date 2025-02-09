#include "AssetManager.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <thread>
#include <atomic>
#include <fstream>
#include <future>

#include "ShaderManager.h"

namespace fs = std::filesystem;

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

AssetManager::AssetManager()
    : running_(true), engineShadersPath(fs::absolute(fs::path(SOURCE_DIR) / "src/shaders").string()),
    runtimeShadersPath(fs::absolute(fs::path(SOURCE_DIR) / "ROOT" / "shaders").string()),
    sourcePath(fs::absolute(SOURCE_DIR).string()),
    rootFolder(std::make_shared<AssetItem>("ROOT", AssetType::Folder, (fs::path(SOURCE_DIR) / "ROOT").string())) {

    /*
    std::cout << "[AssetManager] Initializing AssetManager with:\n"
              << "  Source path: " << sourcePath << "\n"
              << "  Root folder: " << rootFolder->getPath() << "\n"
              << "  Engine shaders path: " << engineShadersPath << "\n"
              << "  Runtime shaders path: " << runtimeShadersPath << std::endl;

    std::cout << "rootFolder use_count: " << rootFolder.use_count() << std::endl;*/

    try {
        validatePaths();

        populateShadersFromShaderManager();

     //   scanThread_ = std::thread(&AssetManager::scanUserAssetsLoop, this);

    } catch (const std::exception& e) {
        std::cerr << "[AssetManager] Initialization error: " << e.what() << std::endl;
        throw;
    }
}

AssetManager::~AssetManager() {
    running_ = false;
    if (scanThread_.joinable()) scanThread_.join();
}
void AssetManager::populateShadersFromShaderManager() {
    const auto& shaderMap = ShaderManager::Instance().getShaders();
    for (const auto& [shaderName, shader] : shaderMap) {
        if (shader) {
            shaders[shaderName] = shader;
        }
    }
}

void AssetManager::scanUserAssetsLoop() {
    while (running_) {
       // scanRuntimeShaders();
      //  std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void AssetManager::scanRuntimeShaders() {
    std::cout << "[AssetManager] Scanning runtime shaders directory: " << runtimeShadersPath
              << " in thread " << std::this_thread::get_id() << std::endl;
    // scanDirectory(runtimeShadersPath, false);
}

void AssetManager::scanPredefinedShaders() {
    std::cout << "[AssetManager] Scanning predefined shaders directory: " << engineShadersPath
              << " in thread " << std::this_thread::get_id() << std::endl;
   // scanDirectory(engineShadersPath, true);
}

void AssetManager::validatePaths() {
    if (!fs::exists(sourcePath)) {
        throw std::runtime_error("Source path does not exist: " + sourcePath);
    }
    if (!fs::exists(rootFolder->getPath())) {
        throw std::runtime_error("Root folder path does not exist: " + rootFolder->getPath());
    }
    if (!fs::exists(engineShadersPath)) {
        throw std::runtime_error("Engine shaders path does not exist: " + engineShadersPath);
    }
    if (!fs::exists(runtimeShadersPath)) {
        throw std::runtime_error("Runtime shaders path does not exist: " + runtimeShadersPath);
    }
}

const std::shared_ptr<AssetItem> & AssetManager::getRootFolder() const {
    return rootFolder;
}

void AssetManager::setRootFolder(std::shared_ptr<AssetItem> root) {
    rootFolder = root;
}


const std::unordered_map<std::string, std::shared_ptr<Shader>> & AssetManager::getShaders() const {
    return shaders;
}

void AssetManager::setRootPath(const std::string& path) {
    rootPath = path;
}

std::string AssetManager::getRootPath() const {
    return rootPath;
}

std::shared_ptr<Shader> AssetManager::getShaderByName(const std::string& shaderName) const {
    /*
    auto it = shaderNameToUUID.find(shaderName);
    if (it != shaderNameToUUID.end()) {
        return getShaderByUUID(it->second);
    }*/
    return nullptr;
}

bool AssetManager::removeShader(const std::string& uuidStr) {
    /*
    auto it = shaders.find(uuidStr);
    if (it != shaders.end()) {
        shaders.erase(it);
        std::cout << "Shader with UUID " << uuidStr << " removed." << std::endl;
        return true;
    }*/
    return false;
}

std::shared_ptr<Shader> AssetManager::getShader(const std::string& name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    }
    return nullptr;
}

AssetType AssetManager::determineAssetType(const std::string &extension) {
    if (extension == ".material" || extension == ".mat") {
        return AssetType::Material;
    } else if (extension == ".glsl" || extension == ".shader") {
        return AssetType::PredefinedShader;
    }
    return AssetType::Unknown;
}

bool AssetManager::addAsset(const std::shared_ptr<AssetItem> &asset) {
    /*
    if (asset->getType() == AssetType::Material) {
        const auto material = std::dynamic_pointer_cast<Material>(asset);
        if (materials.find(material->getUUIDStr()) == materials.end()) {
            materials[material->getUUIDStr()] = material;
            return true;
        }
    }*/
    /*
    if (asset->getType() == AssetType::Shader) {
        const auto shader = std::dynamic_pointer_cast<Shader>(asset);
        if (shaders.find(shader->getName()) == shaders.end()) {
            shaders[shader->getName()] = shader;
            return true;
        }
    }*/
    return false;
}


bool AssetManager::addShader(const std::shared_ptr<Shader>& shader) {
    return false;
}