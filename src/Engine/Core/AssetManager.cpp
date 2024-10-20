// AssetManager.cpp
#include "AssetManager.h"
#include <filesystem>
#include <iostream>
#include <thread>
#include <algorithm>
#include <cctype>
#include <regex>
#include <unordered_set>

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}
AssetManager::AssetManager()
    : engineShadersPath("C:/Users/Simeon/Documents/NAV2SFM_Core/shaders"),
      runtimeShadersPath("C:/Users/Simeon/Documents/NAV2SFM_Core/ROOT/shaders"),
      sourcePath(std::filesystem::absolute(std::string(SOURCE_DIR)).string()),
      rootFolder(std::make_shared<AssetItem>("ROOT", AssetType::Folder, sourcePath + "/ROOT")) {
    std::cout << "[AssetManager] Initializing AssetManager with:\n"
              << "  Source path: " << sourcePath << "\n"
              << "  Root folder: " << rootFolder->getPath() << "\n"
              << "  Engine shaders path: " << engineShadersPath << "\n"
              << "  Runtime shaders path: " << runtimeShadersPath << std::endl;
    try {
        validatePaths();
        loadPredefinedShaders();
        scanUserAssets();
        std::cout << "[AssetManager] Initialization complete." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[AssetManager] Initialization error: " << e.what() << std::endl;
        throw;
    }
}

void AssetManager::validatePaths() {
    if (!std::filesystem::exists(rootFolder->getPath())) {
        throw std::runtime_error("ROOT directory does not exist: " + rootFolder->getPath());
    }
    if (!std::filesystem::exists(engineShadersPath)) {
        throw std::runtime_error("Engine shaders directory does not exist: " + engineShadersPath);
    }
    if (!std::filesystem::exists(runtimeShadersPath)) {
        std::cout << "[AssetManager] Runtime shaders directory does not exist. Creating: " << runtimeShadersPath << std::endl;
        std::filesystem::create_directories(runtimeShadersPath);
    }
}
void AssetManager::loadPredefinedShaders() {
   std::string predefinedShaderPath = engineShadersPath;
    std::cout << "[AssetManager] Loading predefined shaders from: " << predefinedShaderPath << std::endl;

    std::unordered_map<std::string, std::pair<std::string, std::string>> shaderPairs;
    std::regex shaderRegex(R"((.*)(vertex|fragment)\.glsl)", std::regex_constants::icase);

    for (const auto& entry : std::filesystem::directory_iterator(predefinedShaderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".glsl") {
            std::string filename = entry.path().filename().string();
            std::smatch matches;
            if (std::regex_match(filename, matches, shaderRegex)) {
                std::string baseName = matches[1];
                std::string shaderType = matches[2];
                std::cout << "Found " << shaderType << " shader: " << filename << ", base name: " << baseName << std::endl;

                // Normalize shaderType to lowercase for comparison
                std::transform(shaderType.begin(), shaderType.end(), shaderType.begin(), ::tolower);

                if (shaderType == "vertex") {
                    shaderPairs[baseName].first = filename;
                } else if (shaderType == "fragment") {
                    shaderPairs[baseName].second = filename;
                }
            }
        }
    }

    for (const auto& pair : shaderPairs) {
        if (!pair.second.first.empty() && !pair.second.second.empty()) {
            std::cout << "[AssetManager] Creating shader with vertex file: " << pair.second.first
                      << " and fragment file: " << pair.second.second << std::endl;
            try {
                auto shader = std::make_shared<Shader>(pair.second.first, pair.second.second);
                shader->setName(pair.first);  // Set shader name to baseName
                if (addShader(shader)) {
                    std::cout << "[AssetManager] Predefined shader added: " << pair.first << std::endl;
                } else {
                    std::cerr << "[AssetManager] Error adding predefined shader: " << pair.first << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "[AssetManager] Exception loading shader: " << pair.first << " - " << e.what() << std::endl;
            }
        } else {
            std::cerr << "[AssetManager] Unpaired shader found: " << pair.first << std::endl;
        }
    }
}
void AssetManager::compileShaders() {
    for (auto& shaderPair : shaders) {
        shaderPair.second->compile();
    }
}
void AssetManager::scanUserAssets() {
    std::cout << "[AssetManager] Scanning user assets in: " << rootFolder->getPath() << std::endl;

    rootFolder->clearChildren();

    scanDirectory(rootFolder->getPath(), rootFolder);

    std::cout << "[AssetManager] Asset tree structure:" << std::endl;
    printAssetTree(rootFolder, 0);
}
void AssetManager::printAssetTree(const std::shared_ptr<AssetItem>& folder, int indent) {
    if (!folder) return;

    std::string indentStr(indent * 2, ' ');
    std::cout << indentStr << "- " << folder->getName()
              << " (Type: " << static_cast<int>(folder->getType())
              << ", Path: " << folder->getPath() << ")" << std::endl;

    for (const auto& child : folder->getChildren()) {
        printAssetTree(child, indent + 1);
    }
}
void AssetManager::resetAssetTree() {}
std::shared_ptr<AssetItem> AssetManager::getRootFolder() const {
    return rootFolder;
}

void AssetManager::scanDirectory(const std::filesystem::path& directoryPath, const std::shared_ptr<AssetItem>& parentFolder) {
     for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_directory()) {
            std::string folderName = entry.path().filename().string();
            auto folderItem = std::make_shared<AssetItem>(folderName, AssetType::Folder, entry.path().string(), parentFolder);
            parentFolder->addChild(folderItem);
            scanDirectory(entry.path(), folderItem);
        } else if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            AssetType assetType = AssetType::Unknown;
            if (extension == ".glsl" || extension == ".shader") {
                assetType = AssetType::Shader;
            } else if (extension == ".mat") {
                assetType = AssetType::Material;
            }

            if (assetType != AssetType::Unknown) {
                std::string assetName = entry.path().stem().string();
                std::shared_ptr<AssetItem> assetItem;

                if (assetType == AssetType::Shader) {
                    auto shader = std::make_shared<Shader>(entry.path().string());
                    shader->setName(assetName);
                    {
                        std::lock_guard<std::mutex> lock(assetMutex);
                        if (shaders.find(shader->getUUIDStr()) == shaders.end()) {
                            shaders[shader->getUUIDStr()] = shader;
                            assetItem = shader;
                        } else {
                            std::cerr << "[AssetManager] Duplicate Shader UUID: " << shader->getUUIDStr()
                                      << " for shader: " << shader->getName() << ". Skipping." << std::endl;
                            continue;
                        }
                    }
                } else if (assetType == AssetType::Material) {
                    auto material = std::make_shared<Material>(entry.path().string());
                    {
                        std::lock_guard<std::mutex> lock(assetMutex);
                        if (materials.find(material->getUUIDStr()) == materials.end()) {
                            materials[material->getUUIDStr()] = material;
                            assetItem = material;
                        } else {
                            std::cerr << "[AssetManager] Duplicate Material UUID: " << material->getUUIDStr()
                                      << " for material: " << material->getName() << ". Skipping." << std::endl;
                            continue;
                        }
                    }
                }

                parentFolder->addChild(assetItem);
            }
        }
    }
}

void AssetManager::setRootPath(const std::string& path) {
    rootPath = path;
    scanUserAssets();  // Only user assets are modifiable, so scan user path
}

std::string AssetManager::getRootPath() const {
    return rootPath;
}






std::string AssetManager::generateUniquePath(const std::shared_ptr<AssetItem>& parentFolder, const std::string& assetName, AssetType type) {
    std::string parentPath = parentFolder->getPath();
    std::cerr << parentPath << std::endl;

    // Define file extension based on asset type
    std::string extension;
    switch (type) {
        case AssetType::Shader:
            extension = ".shader";
        break;
        case AssetType::Material:
            extension = ".material";
        break;
        // Add other cases as needed
        default:
            extension = "";
    }

    // Construct the full path
    std::filesystem::path fullPath = std::filesystem::path(parentPath) / (assetName + extension);

    // Ensure the path is unique (e.g., by appending a number if necessary)
    int counter = 1;
    while (std::filesystem::exists(fullPath)) {
        std::stringstream ss;
        ss << assetName << "_" << counter++ << extension;
        fullPath = std::filesystem::path(parentPath) / ss.str();
    }

    return fullPath.string();
}

void AssetManager::createShader(const std::shared_ptr<AssetItem>& parentFolder, const std::string& shaderName) {
    if (!parentFolder || parentFolder->getType() != AssetType::Folder) {
        return;
    }

    std::string shaderPath = generateUniquePath(parentFolder, shaderName, AssetType::Shader);
    const auto shader = std::make_shared<AssetItem>(shaderName, AssetType::Shader, shaderPath, parentFolder);
    parentFolder->addChild(shader);
}

void AssetManager::createMaterial(const std::shared_ptr<AssetItem>& parentFolder, const std::string& materialName) {
    if (!parentFolder || parentFolder->getType() != AssetType::Folder) {
        std::cerr << "[AssetManager] Failed to create material: Invalid parent folder." << std::endl;
        return;
    }

    // Generate a unique path for the new material
    std::string materialPath = generateUniquePath(parentFolder, materialName, AssetType::Material);
    std::cout << "[AssetManager] Generated material path: " << materialPath << std::endl;

    // Create the .mat file on disk
    std::ofstream materialFile(materialPath);
    if (!materialFile) {
        std::cerr << "[AssetManager] Failed to create material file at: " << materialPath << std::endl;
        return;
    }
    // Optionally, write default content to the material file
    materialFile << "Material: " << materialName << std::endl;
    materialFile.close();
    std::cout << "[AssetManager] Created material file at: " << materialPath << std::endl;

    // Create a Material object
    const auto material = std::make_shared<Material>(materialPath);
    std::cout << "[AssetManager] Created Material object: " << material->getName() << std::endl;

    // Add the material to the parent folder
    parentFolder->addChild(material);
    std::cout << "[AssetManager] Added Material to parent folder: " << parentFolder->getName() << std::endl;

    // Register the material with AssetManager
    addMaterial(material);
    std::cout << "[AssetManager] Registered Material with UUID: " << material->getUUID() << std::endl;

    std::cout << "[AssetManager] Successfully created material: " << materialName << std::endl;
}

std::shared_ptr<Shader> AssetManager::getShaderByUUID(const std::string& uuidStr) const {
    std::lock_guard<std::mutex> lock(assetMutex);
    auto it = shaders.find(uuidStr);
    if (it != shaders.end() && it->second->getType() == AssetType::Shader) {
        return it->second;
    }
    return nullptr;
}

bool AssetManager::hasShader(const std::string& uuidStr) const {
    std::lock_guard<std::mutex> lock(assetMutex);
    return shaders.find(uuidStr) != shaders.end();
}

bool AssetManager::removeShader(const std::string& uuidStr) {
    std::lock_guard<std::mutex> lock(assetMutex);
    auto it = shaders.find(uuidStr);
    if (it != shaders.end()) {
        shaders.erase(it);
        std::cout << "Shader with UUID " << uuidStr << " removed." << std::endl;
        return true;
    }
    return false;
}

const std::unordered_map<std::string, std::shared_ptr<Shader>>& AssetManager::getShaders() const {
    return shaders;
}
std::shared_ptr<Shader> AssetManager::getShader(const std::string& name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    }
    return nullptr;
}
void AssetManager::addMaterial(const std::shared_ptr<Material>& material) {
    materials[boost::uuids::to_string(material->getUUID())] = material;
    std::cout << "[AssetManager] Material " << material->getName() << " added to AssetManager." << std::endl;
}

std::shared_ptr<Material> AssetManager::getMaterialByUUID(const std::string& uuidStr) const {
    std::lock_guard<std::mutex> lock(assetMutex);
    auto it = materials.find(uuidStr);
    if (it != materials.end()) return it->second;
    return nullptr;
}

bool AssetManager::removeMaterial(const std::string& uuidStr) {
    std::lock_guard<std::mutex> lock(assetMutex);
    auto it = materials.find(uuidStr);
    if (it != materials.end()) {
        materials.erase(it);
        std::cout << "Material with UUID " << uuidStr << " removed." << std::endl;
        return true;
    }
    return false;
}

const std::unordered_map<std::string, std::shared_ptr<Material>>& AssetManager::getMaterials() const {
    return materials;
}


bool AssetManager::addShader(const std::shared_ptr<Shader>& shader) {
    if (shader) {
        shaders[shader->getName()] = shader;
        return true;
    }
    return false;
}
