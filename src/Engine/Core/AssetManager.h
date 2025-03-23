//
// Created by Simeon on 9/29/2024.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <unordered_map>
#include <string>
#include "AssetItem.h"
#include "Shader.h"
#include <filesystem>
#include <thread>
#include <unordered_set>

#include "ShaderData.h"

class AssetManager {
public:
    static AssetManager &getInstance();

    AssetManager();
    ~AssetManager();

    // ================================================================================================
    bool HasAsset(const std::string& uuidStr) {return assets.find(uuidStr) != assets.end();}
    bool addShader(const std::shared_ptr<Shader>& shader);
    std::shared_ptr<Shader> getShaderByUUID(const std::string &uuidStr) const;
    bool hasShader(const std::string &uuidStr) const;
    std::shared_ptr<Shader> getShader(const std::string& name);
    bool removeShader(const std::string &uuidStr);
    const std::unordered_map<std::string, std::shared_ptr<Shader> > &getShaders() const;
    std::shared_ptr<Shader> getShaderByName(const std::string& shaderName) const;

    // ================================================================================================
    AssetType determineAssetType(const std::string& extension);
    bool addAsset(const std::shared_ptr<AssetItem>& asset);
    void scanAssets(const std::string &directoryPath);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    std::string getRootPath() const;
    void setRootPath(const std::string& path);

    void scanUserAssets();
    void validatePaths();

    void debugPrintAssets() const {
        for (const auto& shaderPair : shaders) {
       //     shaderPair.second->debugPrint();
        }
    }
    bool isMainThread() const;

    void scanUserAssetsLoop();
    void scanRuntimeShaders();
    void scanPredefinedShaders();
    void scanDirectory(const std::string& directory, bool isPredefined);

    void loadShaderData(const ShaderData& shaderData);
    void populateShadersFromShaderManager();
    std::shared_ptr<AssetItem> getRootAsset() {
        return rootAsset;
    }

    // Loads assets from the specified directory by scanning it and building the asset tree.
    void loadAssetsFromDirectory(const std::string& directoryPath) {
        // Create a new root asset. "Root" is the display name for the root folder.
        rootAsset = std::make_shared<AssetItem>("Root", AssetType::Folder, directoryPath);
        rootAsset->ScanDirectory(directoryPath);

        // Optionally, set the selected asset to the root after loading.
        selectedAsset = rootAsset;
    }
    void initializeRoot();

    std::shared_ptr<AssetItem> getRootFolder() {
        std::lock_guard<std::mutex> lock(mutex_);
        return rootFolder;
    }

    void setRootFolder(const std::shared_ptr<AssetItem>& root);

    void registerVirtualAsset(const std::shared_ptr<AssetItem>& asset) {
        std::lock_guard<std::mutex> lock(virtualAssetsMutex);
        virtualAssets.push_back(asset);
    }

    std::vector<std::weak_ptr<AssetItem>> getVirtualAssets() const {
        std::lock_guard<std::mutex> lock(virtualAssetsMutex);
        return virtualAssets;
    }

private:
    std::vector<std::weak_ptr<AssetItem>> virtualAssets;
    mutable std::mutex virtualAssetsMutex;

    std::unordered_set<std::string> scannedShaders_;
    mutable std::mutex scannedShadersMutex_;
    std::mutex mutex_;

    std::thread scanThread_;
    std::atomic<bool> running_;

    std::string engineShadersPath;
    std::string runtimeShadersPath;
    std::string sourcePath;

    std::unordered_map<std::string, std::shared_ptr<AssetItem>> assets;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;

    std::vector<std::shared_ptr<Shader>> runtimeShaders;
    std::vector<std::shared_ptr<Shader>> predefinedShaders;
   // std::unordered_map<std::string, std::shared_ptr<Material> > materials;
    std::unordered_map<std::string, std::string> shaderNameToUUID;

    std::shared_ptr<AssetItem> rootFolder;
    std::string rootPath;
    std::thread::id mainThreadId;

    // The root of the asset tree.
    std::shared_ptr<AssetItem> rootAsset;
    // The currently selected asset.
    std::shared_ptr<AssetItem> selectedAsset;
};

#endif //ASSETMANAGER_H
