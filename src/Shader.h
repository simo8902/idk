//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>

class Shader {
public:
    Shader(const char* vertexShaderCode, const char* fragmentShaderCode);
    ~Shader();

    void Use() const;

    GLuint getProgramId();
    void setProgramId(GLuint programId) { shaderProgram = programId; }

    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

    std::string readShaderFile(const std::string& filePath);
    GLuint compileShader(GLenum shaderType, const std::string& shaderSource);
    Shader* createShaderProgram(Shader* shaderProgram);

private:
    GLuint shaderProgram;

};



#endif //SHADER_H
