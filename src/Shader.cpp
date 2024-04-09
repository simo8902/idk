//
// Created by Simeon-PC on 3/28/2024.
//

#include <fstream>
#include <sstream>
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


Shader::~Shader() {
    delete& shaderProgram;
}

Shader *Shader::createShaderProgram(Shader* shaderProgram) {
    std::string vertexShaderSource = shaderProgram->readShaderFile(SOURCE_DIR "/shaders/basic.vert");
    std::string fragmentShaderSource = shaderProgram->readShaderFile(SOURCE_DIR "/shaders/basic.frag");

    shaderProgram = new Shader(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

    GLuint vertexShaderID = shaderProgram->compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    GLuint fragShaderID = shaderProgram->compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    if (vertexShaderID == 0) {
        std::cerr << "Failed to compile vertex shader" << std::endl;
        return nullptr;
    }

    if (fragShaderID == 0) {
        std::cerr << "Failed to compile fragment shader" << std::endl;
        return nullptr;
    }

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderID);
    glAttachShader(programId, fragShaderID);
    glLinkProgram(programId);

    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return nullptr;
    }

    shaderProgram->setProgramId(programId);

    return shaderProgram;
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
    glUseProgram(shaderProgram);
}

GLuint Shader::getProgramId() {
    return shaderProgram;
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
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

    const GLchar* sourceCode = shaderSource.c_str(); // Get raw C-style string
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
    return shader;
}
