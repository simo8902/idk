//
// Created by Simeon-PC on 3/28/2024.
//

#include "Shader.h"

Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
}

void Shader::setMat4(const char* name, const glm::mat4& matrix) {
    GLuint uniformLocation = glGetUniformLocation(shaderProgram, name);
    if (uniformLocation == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader." << std::endl;
        return;
    }

    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::Use() {
    glUseProgram(shaderProgram);
}

GLuint Shader::getProgramId() {
    return shaderProgram;
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}