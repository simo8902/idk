//
// Created by Simeon on 10/2/2024.
//
#include "Material.h"
#include <stdexcept>


// Default Constructor
Material::Material()
    : name("Unnamed Material"), shader(nullptr) {}

// Parameterized Constructor
Material::Material(const std::string& name, const std::shared_ptr<Shader>& shader)
    : name(name), shader(shader) {}

// Copy Constructor
Material::Material(const Material& other)
    : name(other.name), shader(other.shader), textures(other.textures) {}

// Get the name of the material
std::string Material::getName() const {
    return name;
}

// Set the name of the material
void Material::setName(const std::string& newName) {
    name = newName;
}

// Assign a shader to the material
void Material::assignShader(const std::shared_ptr<Shader>& shader) {
    this->shader = shader;
}

// Retrieve the assigned shader
std::shared_ptr<Shader> Material::getShader() const {
    return shader;
}

// Clone the material
std::shared_ptr<Material> Material::clone() const {
    return std::make_shared<Material>(*this);
}