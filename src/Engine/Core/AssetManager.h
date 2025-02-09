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

    const std::shared_ptr<AssetItem> & getRootFolder() const;
    void setRootFolder(std::shared_ptr<AssetItem> root);

    void scanUserAssetsLoop();
    void scanRuntimeShaders();
    void scanPredefinedShaders();
    void scanDirectory(const std::string& directory, bool isPredefined);

    void loadShaderData(const ShaderData& shaderData);
    void populateShadersFromShaderManager();

private:
    std::unordered_set<std::string> scannedShaders_;
    mutable std::mutex scannedShadersMutex_;

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

};

#endif //ASSETMANAGER_H
