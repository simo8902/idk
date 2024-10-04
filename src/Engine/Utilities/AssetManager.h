//
// Created by Simeon on 9/29/2024.
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "Material.h"
#include "Mesh.h"

class AssetManager {
public:
    // Singleton Access
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // Shader Management
    void addShader(const std::string& shaderName, const std::shared_ptr<Shader>& shader);
    std::shared_ptr<Shader> getShader(const std::string& shaderName) const;
    const std::unordered_map<std::string, std::shared_ptr<Shader>>& getShaders() const;

    // Material Management
    void addMaterial(const std::string& materialName, const std::shared_ptr<Material>& material);
    std::shared_ptr<Material> getMaterial(const std::string& materialName) const;
    const std::unordered_map<std::string, std::shared_ptr<Material>>& getMaterials() const;

private:
    // Private Constructor
    AssetManager() = default;

    // Storage for shaders and materials
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
};

#endif //ASSETMANAGER_H
