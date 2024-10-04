//
// Created by Simeon-PC on 3/28/2024.
//
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

Shader::Shader(const std::string& filePath) {
    std::unordered_map<std::string, std::string> shaderSources = ParseShader(filePath);

    CompileShaders(shaderSources["vertex"], shaderSources["fragment"]);
}

// Constructor for separate vertex and fragment shader files
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::unordered_map<std::string, std::string> shaderSources;
    shaderSources["vertex"] = readFile(vertexPath.c_str());
    shaderSources["fragment"] = readFile(fragmentPath.c_str());
    CompileShaders(shaderSources["vertex"], shaderSources["fragment"]);
}

void Shader::Use() const {
    glUseProgram(shaderProgram);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

std::unordered_map<std::string, std::string> Shader::ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);
    if (!stream.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filePath);
    }

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
    ShaderType type = ShaderType::NONE;
    std::unordered_map<std::string, std::string> shaderSources;
    std::stringstream ss[2];

    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            if (type != ShaderType::NONE) {
                ss[static_cast<int>(type)] << line << '\n';
            }
        }
    }

    shaderSources["vertex"] = ss[0].str();
    shaderSources["fragment"] = ss[1].str();

    return shaderSources;
}

std::string Shader::readFile(const char* filePath) {
    std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
    if (fileStream) {
        std::ostringstream contents;
        contents << fileStream.rdbuf();
        fileStream.close();
        return contents.str();
    }
    throw std::runtime_error("Failed to read file: " + std::string(filePath));
}

void Shader::CompileShaders(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* src = vertexSource.c_str();
    glShaderSource(vertexShader, 1, &src, nullptr);
    glCompileShader(vertexShader);
    CheckCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    src = fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &src, nullptr);
    glCompileShader(fragmentShader);
    CheckCompileErrors(fragmentShader, "FRAGMENT");

    // Shader Program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CheckCompileErrors(shaderProgram, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::string errorMsg = "ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog;
            throw std::runtime_error(errorMsg);
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::string errorMsg = "ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog;
            throw std::runtime_error(errorMsg);
        }
    }
}