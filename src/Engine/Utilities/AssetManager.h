//
// Created by Simeon on 9/29/2024.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <boost/uuid/uuid_io.hpp>

#include "Material.h"
#include "Shader.h"
#include "Texture.h"

class Shader;
class Material;
class Mesh;
class Texture;

class AssetManager {
public:
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    // ---------------------
    // Shader Management
    // ---------------------

    void addShader(const std::shared_ptr<Shader>& shader) {
        std::string uuidStr = shader->getUUID();
        shaders.emplace(uuidStr, shader);
        std::cout << "[AssetManager] Shader added: " << shader->getName() << " with UUID: " << uuidStr << std::endl;
        shadersByUUID[shader->getUUID()] = shader;
    }

    std::shared_ptr<Shader> getShader(const std::string& uuidStr) const {
        auto it = shaders.find(uuidStr);
        if (it != shaders.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Get All Shaders
    const std::unordered_map<std::string, std::shared_ptr<Shader>>& getShaders() const {
        return shaders;
    }

    bool hasShader(const std::string& uuidStr) const {
        return shaders.find(uuidStr) != shaders.end();
    }

    void removeShader(const std::string& uuidStr) {
        auto it = shaders.find(uuidStr);
        if (it != shaders.end()) {
            shaders.erase(it);
            std::cout << "[AssetManager] Shader with UUID " << uuidStr << " removed from AssetManager." << std::endl;
        } else {
            std::cerr << "[AssetManager] Error: Shader with UUID " << uuidStr << " not found in AssetManager." << std::endl;
        }
    }
    std::shared_ptr<Shader> getShaderByUUID(const std::string& uuid) const {
        auto it = shadersByUUID.find(uuid);
        if (it != shadersByUUID.end()) {
            return it->second;
        }
        return nullptr;
    }

    // ---------------------
    // Material Management
    // ---------------------

    void addMaterial(const std::shared_ptr<Material>& material) {
        std::string uuidStr = material->getUUID();
        materials.emplace(uuidStr, material);
        std::cout << "[AssetManager] Material added: " << material->getName() << " with UUID: " << uuidStr << std::endl;
    }

    std::shared_ptr<Material> getMaterial(const std::string& uuidStr) const {
        auto it = materials.find(uuidStr);
        if (it != materials.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<Material> getMaterialByUUID(const std::string& uuid) const {
        auto it = materials.find(uuid);
        if (it != materials.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Get All Materials
    const std::unordered_map<std::string, std::shared_ptr<Material>>& getMaterials() const {
        return materials;
    }

    // ---------------------
    // Mesh Management
    // ---------------------

    void addMesh(const std::shared_ptr<Mesh>& mesh) {
        meshes_.push_back(mesh);
        std::cout << "[AssetManager] Mesh added." << std::endl;
    }

    // Get All Meshes
    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const {
        return meshes_;
    }

    // ---------------------
    // Texture Management
    // ---------------------

    void addTexture(const std::shared_ptr<Texture>& texture) {
        std::string uuidStr = texture->getUUID();
        textures.emplace(uuidStr, texture);
        std::cout << "[AssetManager] Texture added: " << texture->getName() << " with UUID: " << uuidStr << std::endl;
    }

    std::shared_ptr<Texture> getTexture(const std::string& uuidStr) const {
        auto it = textures.find(uuidStr);
        if (it != textures.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Get All Textures
    const std::unordered_map<std::string, std::shared_ptr<Texture>>& getTextures() const {
        return textures;
    }

private:
    AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<Mesh>> meshes_;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

    std::unordered_map<std::string, std::shared_ptr<Shader>> shadersByUUID;
};
#endif //ASSETMANAGER_H
