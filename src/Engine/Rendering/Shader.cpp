#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <regex>
#include <filesystem>

#include "AssetManager.h"
#include "glad/glad.h"
#include "gtc/type_ptr.hpp"

#include "Texture.h"

boost::uuids::random_generator Shader::uuidGenerator;

static std::unordered_map<std::string, boost::uuids::uuid> shaderUUIDMap;

Shader::Shader(const std::string& filePath)
    : AssetItem(std::filesystem::path(filePath).stem().string(), AssetType::Shader, filePath),
      vertexPath(""), fragmentPath(filePath) {

    std::cout << "[Shader] Initializing shader with file path: " << filePath
          << " UUID: " << boost::uuids::to_string(getUUID()) << std::endl;

    try {
        parseShaderFile(filePath);
    } catch (const std::exception& e) {
        std::cerr << "[Shader] Error parsing shader file: " << e.what() << std::endl;
        throw;
    }
}
Shader::Shader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
    : AssetItem("PredefinedShader", AssetType::PredefinedShader, "") {

    std::filesystem::path vertexP(vertexShaderFilename);
    std::filesystem::path fragmentP(fragmentShaderFilename);

    // Base path for predefined shaders
    const std::filesystem::path & basePath = std::string(SOURCE_DIR) + "/shaders";

    if (vertexP.is_relative()) {
        vertexP = basePath / vertexP;
    }

    if (fragmentP.is_relative()) {
        fragmentP = basePath / fragmentP;
    }

    vertexPath = vertexP.string();
    fragmentPath = fragmentP.string();
    path = vertexP.parent_path().string();

    std::cout << "[Shader] Initializing predefined shader with vertex path: \"" << vertexPath
              << "\" and fragment path: \"" << fragmentPath << " UUID: " << boost::uuids::to_string(getUUID()) << std::endl;

    loadShaderSources();
}
void Shader::parseShaderFile(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filePath);
    }

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
    ShaderType currentType = ShaderType::NONE;
    std::stringstream ss[2]; // 0: Vertex, 1: Fragment

    std::string line;
    while (std::getline(shaderFile, line)) {
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                currentType = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                currentType = ShaderType::FRAGMENT;
            } else {
                currentType = ShaderType::NONE;
            }
        } else {
            if (currentType != ShaderType::NONE) {
                ss[static_cast<int>(currentType)] << line << '\n';
            }
        }
    }

    vertexSource = ss[0].str();
    fragmentSource = ss[1].str();

    if (vertexSource.empty() || fragmentSource.empty()) {
        throw std::runtime_error("Shader file does not contain both vertex and fragment shaders.");
    }
}
void Shader::loadShaderSources() {
    try {
        if (!vertexPath.empty()) {
            vertexSource = readFile(vertexPath);
        }
        if (!fragmentPath.empty()) {
            fragmentSource = readFile(fragmentPath);
        }
    } catch (const std::exception& e) {
        std::cerr << "[Shader] Exception in loading shader sources: " << e.what() << std::endl;
        throw;
    }
}
void Shader::Use() const {
    glUseProgram(shaderProgram);
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

const std::string& Shader::getName() const {
    return name;
}

void Shader::setName(const std::string& shaderName) {
    name = shaderName;
    std::cout << "[Shader] Shader name set to: " << name << std::endl;
}


std::string Shader::readFile(const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        std::cerr << "Error: File does not exist: " << filePath << std::endl;
        return "";
    }
    std::ifstream stream(filePath, std::ios::in);
    if (!stream.is_open()) {
        std::cerr << "Error: Could not open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << stream.rdbuf();
    return ss.str();
}

void Shader::CompileShaders(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = 0, fragmentShader = 0;

    if (!vertexSource.empty()) {
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* src = vertexSource.c_str();
        glShaderSource(vertexShader, 1, &src, nullptr);
        glCompileShader(vertexShader);
        CheckCompileErrors(vertexShader, "VERTEX");
    }

    if (!fragmentSource.empty()) {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* src = fragmentSource.c_str();
        glShaderSource(fragmentShader, 1, &src, nullptr);
        glCompileShader(fragmentShader);
        CheckCompileErrors(fragmentShader, "FRAGMENT");
    }

    shaderProgram = glCreateProgram();
    if (vertexShader != 0) {
        glAttachShader(shaderProgram, vertexShader);
    }
    if (fragmentShader != 0) {
        glAttachShader(shaderProgram, fragmentShader);
    }
    glLinkProgram(shaderProgram);
    CheckCompileErrors(shaderProgram, "PROGRAM");

    if (vertexShader != 0) {
        glDeleteShader(vertexShader);
    }
    if (fragmentShader != 0) {
        glDeleteShader(fragmentShader);
    }
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "[Shader::CheckCompileErrors] ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "[Shader::CheckCompileErrors] ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}
void Shader::compile() {
    try {
        CompileShaders(vertexSource, fragmentSource);
        std::cout << "[Shader] Shader compiled successfully with UUID: "
                  << boost::uuids::to_string(getUUID())  << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Shader] Exception during shader compilation: " << e.what() << std::endl;
        throw;
    }
}
GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) {
        return it->second;
    }
    GLint location = glGetUniformLocation(shaderProgram, name.c_str());
    if (location == -1) {
        std::string shaderFilePath = "Vertex Shader: " + vertexPath + ", Fragment Shader: " + fragmentPath;
        std::cerr << "[Shader::getUniformLocation] Warning: Uniform '" << name
                  << "' doesn't exist or is not active in shader file(s) '"
                  << shaderFilePath << "'." << std::endl;
    }
    this->uniformLocations[name] = location;
    return location;
}


void Shader::LoadShaderUUIDMap(const std::string& mapFilePath) {
    /*
    std::ifstream inFile(mapFilePath);
    if (!inFile.is_open()) {
        std::cerr << "[Shader] Could not open UUID map file: " << mapFilePath << ". Proceeding with empty map." << std::endl;
        return;
    }

    nlohmann::json jsonMap;
    inFile >> jsonMap;

    for (auto& [path, uuidStr] : jsonMap.items()) {
        try {
            boost::uuids::string_generator gen;
            boost::uuids::uuid u = gen(uuidStr.get<std::string>());
            shaderUUIDMap[path] = u;
        }
        catch (const std::exception& e) {
            std::cerr << "[Shader] Invalid UUID format for path: " << path << ". Error: " << e.what() << std::endl;
        }
    }

    inFile.close();
    std::cout << "[Shader] Loaded shader UUID mapping from " << mapFilePath << std::endl;*/
}

// Function to save UUID mapping
void Shader::SaveShaderUUIDMap(const std::string& mapFilePath) {
    /*
    nlohmann::json jsonMap;
    for (const auto& [path, uuid] : shaderUUIDMap) {
        jsonMap[path] = boost::uuids::to_string(uuid);
    }

    std::ofstream outFile(mapFilePath);
    if (!outFile.is_open()) {
        std::cerr << "[Shader] Could not open UUID map file for writing: " << mapFilePath << std::endl;
        return;
    }

    outFile << jsonMap.dump(4); // Pretty print with 4 spaces
    outFile.close();
    std::cout << "[Shader] Saved shader UUID mapping to " << mapFilePath << std::endl;*/
}


std::unordered_map<std::string, std::string> Shader::ParseShader(const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        throw std::runtime_error("Shader file does not exist: " + filePath);
    }

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

    std::cout << "[Shader::ParseShader] Parsed vertex shader from: " << filePath << std::endl;
    std::cout << "[Shader::ParseShader] Vertex Shader Length: " << shaderSources["vertex"].size() << " characters." << std::endl;
    std::cout << "[Shader::ParseShader] Fragment Shader Length: " << shaderSources["fragment"].size() << " characters." << std::endl;

    return shaderSources;
}
