//
// Created by Simeon on 9/27/2024.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm.hpp"
#include <string>
#include <iostream>
#include <filesystem>
#include "Shader.h"

#include "AssetItem.h"


class Material: public AssetItem  {
public:
    Material(const std::string& name, const std::string& filePath, const bool isPredefined = false)
        : AssetItem(name,  // Използва директно подаденото име
                   AssetType::Material,
                   std::filesystem::path(filePath).parent_path().string(),
                   nullptr),
          shader(nullptr), predefined(isPredefined)
    {
        std::cout << "[Material] Material created: " << getName() << " with UUID: " << getUUIDStr() << std::endl;
    }

    void assignShader(const std::shared_ptr<Shader>& newShader) {
        shader = newShader;
        std::cout << "[Material] Assigned new shader: " << shader->getName() << std::endl;;
    }

    std::shared_ptr<Shader> getShader() const { return shader; }

    std::string name;
    bool isPredefinedMaterial() const {
        if (predefined) {
            std::cout << "[DEBUG] Material '" << this->getName() << "' is predefined." << std::endl;
            return true;
        } else if (std::filesystem::exists(std::filesystem::path(getPath()))) {
            std::cout << "[DEBUG] Material '" << this->getName() << "' is file-based." << std::endl;
            return false;
        } else {
            std::cout << "[DEBUG] Material '" << this->getName() << "' is neither predefined nor file-based." << std::endl;
            return false;
        }
    }
private:
    std::string uuidStr;
    boost::uuids::uuid uuid;
    std::shared_ptr<Shader> shader;
    static boost::uuids::random_generator uuidGenerator;
    bool predefined;
};

#endif //MATERIAL_H
