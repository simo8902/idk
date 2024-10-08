//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include "glm.hpp"

#include <string>
#include <variant>
#include <unordered_map>
#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fstream>
#include "json.hpp"
#include "AssetItem.h"

class Shader: public AssetItem  {
public:
    // Constructor for runtime-created or asset-based shaders
    Shader(const std::string& filePath);

    // Constructor for predefined, code-based shaders
    Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);


    void Use() const;

    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;

    GLuint getProgramID() const {
        if (!predefinedShaders.empty()) {
            // Assuming the shader has a unique name
            auto it = predefinedShaders.find(name);
            if (it != predefinedShaders.end()) {
                return it->second.programID;
            }
        }
        return shaderProgram;
    }

    struct ShaderProgram {
        std::string name;
        GLuint programID;
        boost::uuids::uuid uuid;
    };

    const std::unordered_map<std::string, ShaderProgram>& getPredefinedShaders() const {
        return predefinedShaders;
    }

    const std::string& getName() const;
    void setName(const std::string& shaderName);

    GLint getUniformLocation(const std::string& name) const;

    std::string name;
    void SaveShaderUUIDMap(const std::string& mapFilePath);
    void LoadShaderUUIDMap(const std::string& mapFilePath);
    void loadShaderSources();

    void compile();
    void parseShaderFile(const std::string& filePath);
private:
    // For predefined shaders
    std::unordered_map<std::string, ShaderProgram> predefinedShaders;

    // For runtime shaders
    GLuint shaderProgram = 0;
    std::unordered_map<std::string, std::string> uniforms;

    std::unordered_map<std::string, std::string> ParseShader(const std::string& filePath);
    void CompileShaders(const std::string& vertexSource, const std::string& fragmentSource);
    mutable std::unordered_map<std::string, GLint> uniformLocations;
    static boost::uuids::random_generator uuidGenerator;

    // Utility
    static std::string readFile(const std::string& filePath);

    void CheckCompileErrors(GLuint shader, const std::string& type);
    std::string getCommonPath(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
        return "C:/Users/Simeon/Documents/NAV2SFM_Core/shaders";
    }

    std::filesystem::path resolveShaderPath(const std::string& shaderPath) {
        std::filesystem::path path(shaderPath);

        // Check if the path is relative or absolute
        if (path.is_relative()) {
            // If it's relative, assume it's in the shaders directory
            std::filesystem::path baseDir = "C:/Users/Simeon/Documents/NAV2SFM_Core/shaders";
            path = baseDir / path;
        }

        // Normalize the path to ensure consistent slashes
        return path.lexically_normal();
    }

    // Store shader sources
    std::string vertexSource;
    std::string fragmentSource;
    // Paths to shader files
    std::string vertexPath;
    std::string fragmentPath;
};

#endif // SHADER_H