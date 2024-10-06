#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <regex>

#include "glad/glad.h"
#include "gtc/type_ptr.hpp"

#include "Texture.h"

boost::uuids::random_generator Shader::uuidGenerator;

Shader::Shader(const std::string& filePath) {
    uuid = uuidGenerator();
    uuidStr = boost::uuids::to_string(uuid);

    std::unordered_map<std::string, std::string> shaderSources = ParseShader(filePath);

    CompileShaders(shaderSources["vertex"], shaderSources["fragment"]);

    ExtractUniformsFromSource(shaderSources["fragment"]);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    uuid = uuidGenerator();
    uuidStr = boost::uuids::to_string(uuid);

    std::unordered_map<std::string, std::string> shaderSources;
    shaderSources["vertex"] = readFile(vertexPath.c_str());
    shaderSources["fragment"] = readFile(fragmentPath.c_str());
    CompileShaders(shaderSources["vertex"], shaderSources["fragment"]);

    ExtractUniformsFromSource(shaderSources["fragment"]);
}

void Shader::Use() const {
    glUseProgram(shaderProgram);

    int textureUnit = 0;
    for (const auto& param : parameters) {
        if (param.second.type == ShaderParameterType::Sampler2D) {
            auto texture = std::get_if<std::shared_ptr<Texture>>(&(param.second.value));
            if (texture && *texture) {
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, (*texture)->getID());
                setInt(param.first, textureUnit);
                textureUnit++;
            }
        }
    }
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::addFloatParameter(const std::string& name, float value, float minVal, float maxVal) {
    ShaderParameter param;
    param.name = name;
    param.type = ShaderParameterType::Float;
    param.value = value;
    parameters[name] = param;
}

void Shader::addIntParameter(const std::string& name, int value) {
    ShaderParameter param;
    param.name = name;
    param.type = ShaderParameterType::Int;
    param.value = value;
    parameters[name] = param;
}

void Shader::addVec3Parameter(const std::string& name, float x, float y, float z) {
    ShaderParameter param;
    param.name = name;
    param.type = ShaderParameterType::Vec3;
    param.value = glm::vec3(x, y, z);
    parameters[name] = param;
}

void Shader::addVec4Parameter(const std::string& name, float x, float y, float z, float w) {
    ShaderParameter param;
    param.name = name;
    param.type = ShaderParameterType::Vec4;
    param.value = glm::vec4(x, y, z, w);
    parameters[name] = param;
}

void Shader::addMat4Parameter(const std::string& name, const glm::mat4& matrix) {
    ShaderParameter param;
    param.name = name;
    param.type = ShaderParameterType::Mat4;
    param.value = matrix;
    parameters[name] = param;
}

void Shader::addSampler2DParameter(const std::string& name, std::shared_ptr<Texture> texture) {
    ShaderParameter param;
    param.name = name;
    param.type = ShaderParameterType::Sampler2D;
    param.value = texture;
    parameters[name] = param;
}

void Shader::AddUniformParameter(const std::string& type, const std::string& name) {
    ShaderParameter param;
    param.name = name;

    if (type == "float") {
        param.type = ShaderParameterType::Float;
        param.value = 0.0f;
    }
    else if (type == "int") {
        param.type = ShaderParameterType::Int;
        param.value = 0;
    }
    else if (type == "vec3") {
        param.type = ShaderParameterType::Vec3;
        param.value = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    else if (type == "vec4") {
        param.type = ShaderParameterType::Vec4;
        param.value = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (type == "mat4") {
        param.type = ShaderParameterType::Mat4;
        param.value = glm::mat4(1.0f);
    }
    else if (type == "sampler2D") {
        param.type = ShaderParameterType::Sampler2D;
        param.value = std::shared_ptr<Texture>(nullptr);
    }
    else {
        std::cerr << "Unknown shader parameter type: " << type << " for uniform: " << name << std::endl;
        return;
    }

    parameters[name] = param;
}

const std::string& Shader::getName() const {
    return name;
}

void Shader::setName(const std::string& shaderName) {
    name = shaderName;
    std::cout << "[Shader] Shader name set to: " << name << std::endl;
}

std::unordered_map<std::string, std::string> Shader::ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);
    if (!stream.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filePath);
    }

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
    ShaderType type = ShaderType::NONE;

    std::unordered_map<std::string, std::string> shaderSources;
    std::stringstream ss[2];  // [0] for vertex shader, [1] for fragment shader

    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
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

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CheckCompileErrors(shaderProgram, "PROGRAM");

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

GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) {
        return it->second;
    }
    GLint location = glGetUniformLocation(shaderProgram, name.c_str());
    this->uniformLocations[name] = location;
    return location;
}

ShaderParameter * Shader::getParameter(const std::string &paramName) {
    auto it = parameters.find(paramName);
    if (it != parameters.end()) {
        return &(it->second);
    }
    return nullptr;
}

void Shader::ExtractUniformsFromSource(const std::string &shaderSource) {
    std::regex uniformRegex(R"(uniform\s+(\w+)\s+(\w+)\s*;)");
    std::smatch match;
    std::string::const_iterator searchStart(shaderSource.cbegin());

    while (std::regex_search(searchStart, shaderSource.cend(), match, uniformRegex)) {
        std::string type = match[1];
        std::string name = match[2];

        std::cout << "[DEBUG] Shader: Found uniform '" << name << "' of type '" << type << "'" << std::endl;

        ShaderParameter param;
        param.name = name;
        if (type == "mat4") {
            param.type = ShaderParameterType::Mat4;
            param.value = glm::mat4(1.0f);  // identity matrix
        } else if (type == "vec3") {
            param.type = ShaderParameterType::Vec3;
            param.value = glm::vec3(1.0f, 1.0f, 1.0f);
        } else if (type == "vec4") {
            param.type = ShaderParameterType::Vec4;
            param.value = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        } else if (type == "float") {
            param.type = ShaderParameterType::Float;
            param.value = 1.0f;
        } else if (type == "int") {
            param.type = ShaderParameterType::Int;
            param.value = 0;
        } else if (type == "sampler2D") {
            param.type = ShaderParameterType::Sampler2D;
            param.value = std::shared_ptr<Texture>(nullptr);
        } else if (type == "samplerCube") {
            std::cerr << "[ERROR] Shader: Unrecognized uniform type: samplerCube" << std::endl;
            searchStart = match.suffix().first;
            continue;
        } else if (type == "DirectionalLight") {
            std::cerr << "[ERROR] Shader: Unrecognized uniform type: DirectionalLight" << std::endl;
            searchStart = match.suffix().first;
            continue;
        } else {
            std::cerr << "[ERROR] Shader: Unrecognized uniform type: " << type << std::endl;
            searchStart = match.suffix().first;
            continue;
        }

        parameters[name] = param;
        searchStart = match.suffix().first;
    }
}
