//
// Created by Simeon on 9/27/2024.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm.hpp"
#include <string>
#include <iostream>
#include "Shader.h"

class Material {
public:
    Material(const std::string& materialName)
        : name(materialName), shader(nullptr) {
        uuid = uuidGenerator();
        uuidStr = boost::uuids::to_string(uuid);

        std::cout << "[Material] Material created: " << name << " with UUID: " << uuidStr << std::endl;
    }

    std::string getName() const { return name; }
    std::string getUUID() const { return uuidStr; }

    void assignShader(const std::shared_ptr<Shader>& newShader) {
        /*
        if (shader) {
            std::cout << "[Material] Replacing shader: " << shader->getName() << " with new shader: " << newShader->getName() << std::endl;
        } else {
            std::cout << "[Material] Assigning first shader: " << newShader->getName() << std::endl;
        }*/

        shader = newShader;
        parameters.clear();
        std::cout << "[Material] Assigned new shader: " << shader->getName() << std::endl;;
        ExtractShaderParameters();
    }

    std::shared_ptr<Shader> getShader() const { return shader; }

    void setParameter(const std::string& paramName, const ShaderParameter& param) {
        parameters[paramName] = param;
    }

    ShaderParameter* getParameter(const std::string& paramName) {
        auto it = parameters.find(paramName);
        if (it != parameters.end()) {
            return &(it->second);
        }

        if (shader) {
            ShaderParameter* shaderParam = shader->getParameter(paramName);
            if (shaderParam) {
                parameters[paramName] = *shaderParam;
                return &(parameters[paramName]);
            }
        }
        return nullptr;
    }

    const std::unordered_map<std::string, ShaderParameter>& getParameters() const {
        return parameters;
    }
    void ExtractShaderParameters() {
        if (!shader) return;

        for (const auto& [paramName, shaderParam] : shader->getParameters()) {
            if (paramName == "baseColor") {
                ShaderParameter baseColorParam;
                baseColorParam.name = "baseColor";
                baseColorParam.type = ShaderParameterType::Vec3;
                baseColorParam.value = glm::vec3(1.0f, 1.0f, 1.0f);
                parameters["baseColor"] = baseColorParam;
            } else if (paramName == "albedoTexture") {
                ShaderParameter albedoTextureParam;
                albedoTextureParam.name = "albedoTexture";
                albedoTextureParam.type = ShaderParameterType::Sampler2D;
                albedoTextureParam.value = std::shared_ptr<Texture>(nullptr);
                parameters["albedoTexture"] = albedoTextureParam;
            } else {
                parameters[paramName] = shaderParam;
            }
        }
    }

    glm::vec3 getBaseColor() const {
        auto it = parameters.find("baseColor");
        if (it != parameters.end()) {
            if (std::holds_alternative<glm::vec3>(it->second.value)) {
                return std::get<glm::vec3>(it->second.value);
            } else {
                std::cerr << "[Material] 'baseColor' parameter is not of type Vec3!" << std::endl;
            }
        }
        return glm::vec3(1.0f, 1.0f, 1.0f);
    }
    void setBaseColor(const glm::vec3& color) {
        if (parameters.find("baseColor") != parameters.end()) {
            parameters["baseColor"].value = color;
        } else {
            ShaderParameter param;
            param.name = "baseColor";
            param.type = ShaderParameterType::Vec3;
            param.value = color;
            parameters["baseColor"] = param;
        }

        std::cout << "[DEBUG] Material - baseColor set to: ("  << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;

        if (shader) {
            shader->Use();
            shader->setVec3("baseColor", color);
            std::cout << "[DEBUG] Shader baseColor uniform updated." << std::endl;
        } else {
            std::cerr << "[ERROR] No shader assigned to material. Cannot update shader uniform." << std::endl;
        }
    }
private:
    std::string name;
    std::string uuidStr;

    boost::uuids::uuid uuid;
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, ShaderParameter> parameters;

    static boost::uuids::random_generator uuidGenerator;

    void initializeShaderParameters()
    {
        if (shader) {
            for (const auto& [paramName, param] : shader->getParameters())
                {
                if (parameters.find(paramName) == parameters.end()) {
                    parameters[paramName] = param;
                }
            }
        }
    }
};

#endif //MATERIAL_H
