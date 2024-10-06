//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include "glm.hpp"

#include <string>
#include <variant>
#include <unordered_map>
#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class Texture;

enum class ShaderParameterType {
    Float,
    Int,
    Vec3,
    Vec4,
    Mat4,
    Sampler2D
};

struct ShaderParameter {
    std::string name;
    ShaderParameterType type;
    std::variant<float, int, glm::vec3, glm::vec4, glm::mat4, std::shared_ptr<Texture>> value;
};

class Shader {
public:
    explicit Shader(const std::string& filePath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    void Use() const;

    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;

    GLuint GetProgramID() const { return shaderProgram; }

    const std::unordered_map<std::string, ShaderParameter>& getParameters() const { return parameters; }

    void addFloatParameter(const std::string& name, float value, float minVal = 0.0f, float maxVal = 1.0f);
    void addIntParameter(const std::string& name, int value);
    void addVec3Parameter(const std::string& name, float x, float y, float z);
    void addVec4Parameter(const std::string& name, float x, float y, float z, float w);
    void addMat4Parameter(const std::string& name, const glm::mat4& matrix);
    void addSampler2DParameter(const std::string& name, std::shared_ptr<Texture> texture = nullptr);

    void AddUniformParameter(const std::string& type, const std::string& name);

    const std::string& getName() const;
    void setName(const std::string& shaderName);

    GLint getUniformLocation(const std::string& name) const;

    std::string  getUUID() const { return uuidStr; }

    void addParameter(const ShaderParameter& param) {
        parameters[param.name] = param;
    }

    ShaderParameter* getParameter(const std::string& paramName);
    const std::unordered_map<std::string, std::string>& getUniforms() const {
        return uniforms;
    }
    void ExtractUniformsFromSource(const std::string& fragmentSource);

private:
    GLuint shaderProgram;
    std::unordered_map<std::string, ShaderParameter> parameters;
    std::unordered_map<std::string, std::string> uniforms;

    std::string name;
    std::string uuidStr;
    boost::uuids::uuid uuid;

    std::unordered_map<std::string, std::string> ParseShader(const std::string& filePath);
    static std::string readFile(const char* filePath);
    void CompileShaders(const std::string& vertexSource, const std::string& fragmentSource);
    void CheckCompileErrors(GLuint shader, const std::string& type);

    mutable std::unordered_map<std::string, GLint> uniformLocations;

    static boost::uuids::random_generator uuidGenerator;
};

#endif //NAV2SFM Core_SHADER_H