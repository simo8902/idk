//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "glm.hpp"
#include "glad/glad.h"

#include <iostream>
#include <memory>

class Shader {
public:
    Shader() = default;
    ~Shader() = default;
    unsigned int shaderProgram{};

    Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
    void Use() const;
    void setMat4(const std::string &name, const glm::mat4 &matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

    [[nodiscard]] GLuint GetProgramID() const {
        return shaderProgram;
    }
private:
    static std::string readFile(const char* filePath);
    static void checkCompileStatus(unsigned int shader, const std::string& type);

};

#endif //SHADER_H