#include "AssetManager.h"
#include <stdexcept>

// Shader Management

void AssetManager::addShader(const std::string& shaderName, const std::shared_ptr<Shader>& shader) {
    if (shaders.find(shaderName) != shaders.end()) {
        throw std::runtime_error("Shader already exists: " + shaderName);
    }
    shaders.emplace(shaderName, shader);
}

std::shared_ptr<Shader> AssetManager::getShader(const std::string& shaderName) const {
    auto it = shaders.find(shaderName);
    if (it != shaders.end()) {
        return it->second;
    }
    throw std::runtime_error("Shader not found: " + shaderName);
}

const std::unordered_map<std::string, std::shared_ptr<Shader>>& AssetManager::getShaders() const {
    return shaders;
}

// Material Management

void AssetManager::addMaterial(const std::string& materialName, const std::shared_ptr<Material>& material) {
    if (materials.find(materialName) != materials.end()) {
        throw std::runtime_error("Material already exists: " + materialName);
    }
    materials.emplace(materialName, material);
}

std::shared_ptr<Material> AssetManager::getMaterial(const std::string& materialName) const {
    auto it = materials.find(materialName);
    if (it != materials.end()) {
        return it->second;
    }
    throw std::runtime_error("Material not found: " + materialName);
}

const std::unordered_map<std::string, std::shared_ptr<Material>>& AssetManager::getMaterials() const {
    return materials;
}