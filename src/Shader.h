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
    GLuint shaderProgram;

    Shader() : shaderProgram(0) {}
    ~Shader() {
        glDeleteProgram(shaderProgram);
    }

    static std::shared_ptr<Shader> createShaderProgram();
    static std::string readShaderFile(const std::string& filePath);
    static GLuint compileShader(GLenum shaderType, const std::string& shaderSource);

    void setMat4(const std::string &name, const glm::mat4 &matrix) const;
    void Use() const;
    GLuint getProgramId() const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
};

#endif //SHADER_H