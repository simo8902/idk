//
// Created by simeon on 24.01.25.
//

#include <fstream>
#include "Shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath, const std::string& name)
    : vertexShaderPath(vertexPath),
      fragmentShaderPath(fragmentPath),
      AssetItem(name, AssetType::Shader, std::string(vertexPath))
{
    id = boost::uuids::random_generator()();

    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    checkCompileStatus(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    checkCompileStatus(fragmentShader, "FRAGMENT");

    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vertexShader);
    glAttachShader(m_ProgramID, fragmentShader);
    glLinkProgram(m_ProgramID);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &matrix) const {
    GLuint uniformLocation = glGetUniformLocation(m_ProgramID, name.c_str());
    if (uniformLocation == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader." << std::endl;
        return;
    }
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, &value[0]);
}

std::string Shader::readFile(const char *filePath) {
    std::ifstream shaderFile(filePath);
    return std::string((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
}
GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) return it->second;
    GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
    if (location == -1) {
        std::cerr << "[Shader::getUniformLocation] Warning: Uniform '" << name
                  << "' doesn't exist or is not active in shader file(s) "
                  << "vertex shader: '" << vertexShaderPath << "' and fragment shader: '" << fragmentShaderPath << "'." << std::endl;
    }
    this->uniformLocations[name] = location;
    return location;
}
void Shader::checkCompileStatus(unsigned int shader, const std::string &type) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader compilation failed for type: " << type << "\n" << infoLog << std::endl;
    }
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}