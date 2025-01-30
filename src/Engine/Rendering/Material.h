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
        : AssetItem(name, AssetType::Material, filePath),
          shader(nullptr), predefined(isPredefined) {
        std::cout << "[Material] Material created: " << getName() << " with UUID: " << getUUIDStr() << std::endl;
    }

    void assignShader(const std::shared_ptr<Shader>& newShader) {
        shader = newShader;
        //std::cout << "[Material] Assigned new shader: " << shader->getName() << std::endl;;
    }

    std::shared_ptr<Shader> getShader() const { return shader; }

    std::string name;
    bool isPredefinedMaterial() const {
        static bool debugMessagePrinted = false;

        if (predefined) {
            if (!debugMessagePrinted) {
                std::cout << "[DEBUG] Material '" << this->getName() << "' is predefined." << std::endl;
                debugMessagePrinted = true;
            }
            return true;
        } else if (std::filesystem::exists(std::filesystem::path(getPath()))) {
            if (!debugMessagePrinted) {
                std::cout << "[DEBUG] Material '" << this->getName() << "' is file-based." << std::endl;
                debugMessagePrinted = true;
            }
            return false;
        } else {
            if (!debugMessagePrinted) {
                std::cout << "[DEBUG] Material '" << this->getName() << "' is neither predefined nor file-based." << std::endl;
                debugMessagePrinted = true;
            }
            return false;
        }
    }
private:
    std::string uuidStr;
    std::shared_ptr<Shader> shader;
    bool predefined;
};

#endif //MATERIAL_H
