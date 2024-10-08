//
// Created by Simeon on 9/29/2024.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <filesystem>
#include <unordered_set>

#include "AssetItem.h"
#include "Material.h"
#include "Shader.h"

class AssetManager {
public:
    static AssetManager &getInstance();

    // ================================================================================================
    bool HasAsset(const std::string& uuidStr) {return assets.find(uuidStr) != assets.end();}
    bool addShader(const std::shared_ptr<Shader>& shader);
    std::shared_ptr<Shader> getShaderByUUID(const std::string &uuidStr) const;
    bool hasShader(const std::string &uuidStr) const;
    std::shared_ptr<Shader> getShader(const std::string& name);
    bool removeShader(const std::string &uuidStr);
    const std::unordered_map<std::string, std::shared_ptr<Shader> > &getShaders() const;
    bool addMaterial(std::shared_ptr<Material> material);
    std::shared_ptr<Material> getMaterialByUUID(const std::string &uuidStr) const;
    bool removeMaterial(const std::string &uuidStr);
    const std::unordered_map<std::string, std::shared_ptr<Material> > &getMaterials() const;
    // ================================================================================================

    void compileShaders();
    void scanAssets(const std::string &directoryPath);

    std::shared_ptr<AssetItem> getRootFolder() const;
    mutable std::mutex assetMutex;
    //void scanAssetsAsync(const std::string& directoryPath);
    std::shared_ptr<AssetItem> findFolder(const std::string& path, const std::shared_ptr<AssetItem>& parent);
    void resetAssetTree();

    std::string getRootPath() const;
    void setRootPath(const std::string& path);
    void loadPredefinedShaders();
    void scanUserAssets();
    void scanDirectory(const std::filesystem::path& directoryPath, const std::shared_ptr<AssetItem>& parentFolder);
    void validatePaths();
    void printAssetTree(const std::shared_ptr<AssetItem>& folder, int indent);

    void createFolder(const std::shared_ptr<AssetItem>& parentFolder, const std::string& folderName) {
        std::string newFolderPath = parentFolder->getPath() + "/" + folderName;
        if (std::filesystem::create_directory(newFolderPath)) {
            auto newFolder = std::make_shared<AssetItem>(folderName, AssetType::Folder, newFolderPath, parentFolder);
            parentFolder->addChild(newFolder);
            std::cout << "[AssetManager] Folder created: " << newFolderPath << std::endl;
        } else {
            std::cerr << "[AssetManager] Failed to create folder: " << newFolderPath << std::endl;
        }
    }
private:
    std::mutex shadersMutex;
    std::string engineShadersPath;
    std::string runtimeShadersPath;
    std::string sourcePath;

    std::string normalizePath(const std::string& path) {
        if (path.empty()) {
            throw std::invalid_argument("Path cannot be empty");
        }

        std::filesystem::path fs_path(path);
        fs_path = fs_path.lexically_normal();

        // Convert to absolute path if it's not already
        if (!fs_path.is_absolute()) {
            fs_path = std::filesystem::absolute(fs_path);
        }

        // Use generic_string() for consistent path separators across platforms
        return fs_path.generic_string();
    }

    AssetManager();
    AssetManager(const AssetManager &) = delete;
    std::unordered_map<std::string, std::shared_ptr<AssetItem>> assets;
    AssetManager &operator=(const AssetManager &) = delete;

    std::unordered_map<std::string, std::shared_ptr<Shader> > shaders;
    std::unordered_map<std::string, std::shared_ptr<Material> > materials;

    std::shared_ptr<AssetItem> rootFolder;
    std::string rootPath;

};

#endif //ASSETMANAGER_H
