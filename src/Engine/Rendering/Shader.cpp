//
// Created by simeon on 24.01.25.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.h"

#include <filesystem>
#include <chrono>
#include <ctime>

Shader::Shader(const char* path, bool isCombined) : isCombined(isCombined) {
    if (isCombined) {
        auto [vertexCode, fragmentCode] = parseCombinedShader(path);
        compileAndLink(vertexCode, fragmentCode);
        currentPaths = {path, path};
    }
}


Shader::Shader(const char* vertexPath, const char* fragmentPath) : isCombined(false) {
    if (!std::filesystem::exists(vertexPath)) {
        throw std::runtime_error("Vertex shader file does not exist: " + std::string(vertexPath));
    }
    if (!std::filesystem::exists(fragmentPath)) {
        throw std::runtime_error("Fragment shader file does not exist: " + std::string(fragmentPath));
    }

    compileAndLink(readFile(vertexPath), readFile(fragmentPath));

    currentPaths = {vertexPath, fragmentPath};
}
Shader::~Shader() {
    if(shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
    }
}
void Shader::loadCombinedShader(const char* path) {
    auto [vertexCode, fragmentCode] = parseCombinedShader(path);
    compileAndLink(vertexCode, fragmentCode);
    currentPaths = {path, path};
    isCombined = true;
}

void Shader::loadSeparateShaders(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    compileAndLink(vertexCode, fragmentCode);
    currentPaths = {vertexPath, fragmentPath};
    isCombined = false;
}

std::pair<std::string, std::string> Shader::parseCombinedShader(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::string line;
    std::string vertexCode;
    std::string fragmentCode;
    bool isVertex = false;
    bool isFragment = false;

    while (std::getline(file, line)) {
        if (line.find("#type vertex") != std::string::npos) {
            isVertex = true;
            isFragment = false; // Reset fragment flag
            continue; // Skip this line
        }
        if (line.find("#type fragment") != std::string::npos) {
            isFragment = true;
            isVertex = false; // Reset vertex flag
            continue; // Skip this line
        }

        // Append code to the appropriate string
        if (isVertex) {
            vertexCode += line + "\n";
        } else if (isFragment) {
            fragmentCode += line + "\n";
        }
    }

    return {vertexCode, fragmentCode};

}


void Shader::reload() {
    if(isCombined) {
        loadCombinedShader(currentPaths.vertex.c_str());
    } else {
        loadSeparateShaders(currentPaths.vertex.c_str(), currentPaths.fragment.c_str());
    }
}

void Shader::reloadFromPath(const std::string& path) {
    if(path.ends_with(".glsl")) {
        loadCombinedShader(path.c_str());
    } else if(path.ends_with(".vert")) {
        currentPaths.vertex = path;
        reload();
    } else if(path.ends_with(".frag")) {
        currentPaths.fragment = path;
        reload();
    }
}

void Shader::compileAndLink(const std::string& vertexCode, const std::string& fragmentCode) {
    GLuint vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);

        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";

        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
        throw std::runtime_error("Shader program linking failed.");
    } else {
       // std::cout << "Successfully linked shader program.\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLuint Shader::compileShader(const std::string& source, GLenum type) {
    if (source.empty()) {
        throw std::runtime_error("Empty shader source");
    }

    const char* src = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

        // Log detailed error message
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: "
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << "\n" << infoLog << "\n";

        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed.");
    } else {
        /*
        std::cout << "Successfully compiled "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader.\n";*/
    }

    return shader;
}


void Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "SHADER COMPILATION ERROR (" << type << "):\n" << infoLog << std::endl;
        glDeleteShader(shader);
    }
}
bool Shader::checkCompileStatus(GLuint shader, const std::string& type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "SHADER COMPILATION ERROR (" << type << "):\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

std::string Shader::getLastModified() const {
    std::filesystem::path vertexPath(currentPaths.vertex);
    std::filesystem::path fragmentPath(currentPaths.fragment);

    std::string lastModified;

    try {
        // Get the last write time for both files
        auto vertexTime = std::filesystem::last_write_time(vertexPath);
        auto fragmentTime = std::filesystem::last_write_time(fragmentPath);

        // Convert file_time_type to system_clock time_point
        auto systemVertexTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            vertexTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

        auto systemFragmentTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            fragmentTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

        // Convert to time_t for easier formatting
        auto cftimeVertex = std::chrono::system_clock::to_time_t(systemVertexTime);
        auto cftimeFragment = std::chrono::system_clock::to_time_t(systemFragmentTime);

        // Format the time as a string
        std::ostringstream oss;
        oss << "Vertex: " << std::put_time(std::localtime(&cftimeVertex), "%Y-%m-%d %H:%M:%S")
            << ", Fragment: " << std::put_time(std::localtime(&cftimeFragment), "%Y-%m-%d %H:%M:%S");

        lastModified = oss.str();
    } catch (const std::exception& e) {
        lastModified = "Error retrieving time: " + std::string(e.what());
    }

    return lastModified;

}

void Shader::checkLinkStatus(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "SHADER LINKING ERROR:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        throw std::runtime_error("Shader program linking failed");
    }
}


std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if(!file.is_open()) throw std::runtime_error("Failed to open file: " + path);

    size_t fileSize = file.tellg();
    std::string buffer(fileSize, ' ');
    file.seekg(0);
    file.read(&buffer[0], fileSize);
    file.close();
    return buffer;
}


// Uniform setters remain similar but could benefit from caching
void Shader::Use() const {
    glUseProgram(shaderProgram);
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, glm::value_ptr(value));
}