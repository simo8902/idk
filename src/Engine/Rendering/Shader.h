//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "glm.hpp"
#include "glad/glad.h"

#include <memory>
#include <unordered_map>
#include <string>

class Shader {
public:
    explicit Shader(const std::string& filePath);

    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void Use() const;

    void setMat4(const std::string &name, const glm::mat4 &matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setInt(const std::string &name, int value) const;

    GLuint GetProgramID() const { return shaderProgram; }

private:
    GLuint shaderProgram;

    std::unordered_map<std::string, std::string> ParseShader(const std::string& filePath);

    static std::string readFile(const char* filePath);

    void CompileShaders(const std::string& vertexSource, const std::string& fragmentSource);

    void CheckCompileErrors(GLuint shader, const std::string& type);

};

#endif //NAV2SFM Core_SHADER_H