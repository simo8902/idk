//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "libs/glad/include/glad/glad.h"

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>

class Shader {
public:
    Shader(const char* vertexShaderCode, const char* fragmentShaderCode);
    void Use();
    GLuint getProgramId();

    void setMat4(const char* name, const glm::mat4& matrix);
    void setVec3(const std::string& name, const glm::vec3& value) const;
private:
    GLuint shaderProgram;

};



#endif //SHADER_H
