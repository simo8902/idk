//
// Created by Simeon on 9/27/2024.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "glad/glad.h"
#include "glm.hpp"
#include <string>
#include <iostream>
#include "Shader.h"

class Material {
public:
    Material();
    Material(const std::string& name, const std::shared_ptr<Shader>& shader = nullptr);
    Material(const Material& other); // Copy constructor
    ~Material() = default;

    std::string getName() const;
    void setName(const std::string& newName);

    void assignShader(const std::shared_ptr<Shader>& shader);
    std::shared_ptr<Shader> getShader() const;

    std::shared_ptr<Material> clone() const;

private:
    std::string name;
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, GLuint> textures;
};

#endif //MATERIAL_H
