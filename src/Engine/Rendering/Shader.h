//
// Created by simeon on 24.01.25.
//

#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>


class Shader {
public:
    struct Paths {
        std::string vertex;
        std::string fragment;
    };

    Shader(const char* path, bool isCombined = true);
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    GLuint getProgramID() const { return shaderProgram; }

    void Use() const;
    void reload();
    void reloadFromPath(const std::string& path);

    // Uniform setters
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setInt(const std::string& name, int value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

    // State management
    bool isValid() const { return shaderProgram != 0; }
    std::string getLastModified() const;
    const Paths& getPaths() const { return currentPaths; }

private:
    GLuint shaderProgram = 0;
    bool isCombined;
    Paths currentPaths;
    mutable std::unordered_map<std::string, GLint> uniformCache;
    mutable std::mutex uniformMutex;

    void compileAndLink(const std::string& vertexCode, const std::string& fragmentCode);
    GLuint compileShader(const std::string& source, GLenum type);
    std::pair<std::string, std::string> parseCombinedShader(const std::string& path);
    std::string readFile(const std::string& path);
    void checkCompileErrors(GLuint shader, const std::string& type);
    bool checkCompileStatus(GLuint shader, const std::string& type);
    void loadSeparateShaders(const char* vertexPath, const char* fragmentPath);
    void loadCombinedShader(const char* path);
    void checkLinkStatus(GLuint program);

};

#endif //SHADER_H
