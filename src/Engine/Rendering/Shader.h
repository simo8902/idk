//
// Created by simeon on 24.01.25.
//

#ifndef SHADER_H
#define SHADER_H

#include <AssetItem.h>

#include "glad/glad.h"

#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include <unordered_map>

class Shader : public AssetItem {
public:

    unsigned int m_ProgramID;
    GLuint getProgramID() const { return m_ProgramID; }

    Shader(const char *vertexShaderPath, const char *fragmentShaderPath, const std::string& name);
    ~Shader() {
        if (m_ProgramID != 0) {
            glDeleteProgram(m_ProgramID);
        }
    }

    void Use() const {
        glUseProgram(m_ProgramID);
    }
    void setMat4(const std::string &name, const glm::mat4 &matrix) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    GLint getUniformLocation(const std::string& name) const;

private:
    std::string vertexShaderPath;
    std::string fragmentShaderPath;

    static std::string readFile(const char* filePath);
    static void checkCompileStatus(unsigned int shader, const std::string& type);

    mutable std::unordered_map<std::string, GLint> uniformLocations;
};

#endif //SHADER_H
