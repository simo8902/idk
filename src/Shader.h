//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>
#include <memory>

class Shader {
public:

    unsigned int shaderProgram;

    Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
    void Use() const {
        glUseProgram(shaderProgram);
    }
    void setMat4(const std::string &name, const glm::mat4 &matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

private:
    static std::string readFile(const char* filePath);
    static void checkCompileStatus(unsigned int shader, const std::string& type);

};

#endif //SHADER_H