//
// Created by Simeon-PC on 3/28/2024.
//

#include <fstream>
#include <sstream>
#include "Shader.h"

std::shared_ptr<Shader> Shader::createShaderProgram() {
    std::string vertexShaderSource = readShaderFile(SOURCE_DIR "/shaders/basic.vert");
    std::string fragmentShaderSource = readShaderFile(SOURCE_DIR "/shaders/basic.frag");
    auto newShaderProgram = std::make_shared<Shader>();

    GLuint vertexShaderID = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragShaderID = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    if (vertexShaderID == 0 || fragShaderID == 0) {
        std::cerr << "Shader compilation failed, aborting shader program creation." << std::endl;
        return nullptr;
    }

    newShaderProgram->shaderProgram = glCreateProgram();
    glAttachShader(newShaderProgram->shaderProgram, vertexShaderID);
    glAttachShader(newShaderProgram->shaderProgram, fragShaderID);
    glLinkProgram(newShaderProgram->shaderProgram);

    GLint success;
    glGetProgramiv(newShaderProgram->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(newShaderProgram->shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return nullptr;
    }

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragShaderID);

    return newShaderProgram;
}

std::string Shader::readShaderFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(GLenum shaderType, const std::string& shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    const GLchar* sourceCode = shaderSource.c_str();
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    std::cout << "Shader compilation successful: " << (shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << std::endl;
    return shader;
}

void Shader::setMat4(const std::string &name, const glm::mat4 &matrix) const {
    GLuint uniformLocation = glGetUniformLocation(shaderProgram, name.c_str());
    if (uniformLocation == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader." << std::endl;
        return;
    }
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::Use() const {
    if (shaderProgram == 0){
        std::cerr << "Shader program is not created\n";
        return;
    }
    glUseProgram(shaderProgram);
}

GLuint Shader::getProgramId() const {
    return shaderProgram;
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}
