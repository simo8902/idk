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

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void Use() const;
    void setMat4(const std::string &name, const glm::mat4 &matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

    GLuint GetProgramID() const {
        return shaderProgram;
    }
    void setInt(const std::string &name, int value) {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }
private:
    static std::string readFile(const char* filePath);
    static void checkCompileStatus(unsigned int shader, const std::string& type);
    std::string vertexShaderFilename;
    std::string fragmentShaderFilename;

};

#endif //NAV2SFM Core_SHADER_H