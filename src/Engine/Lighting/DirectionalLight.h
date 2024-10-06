//
// Created by Simeon on 9/22/2024.
//

#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "glad/glad.h"
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "Light.h"
#include <iostream>

class DirectionalLight : public Light {
public:
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirectionalLight(const std::string& name, const glm::vec3& dir, const glm::vec3& amb,
          const glm::vec3& diff, const glm::vec3& spec)
        : Light(name), direction(glm::normalize(dir)), ambient(amb), diffuse(diff), specular(spec) {
    }

    void setUniforms(unsigned int shaderProgram) const override {
        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.direction"), 1, glm::value_ptr(direction));
        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 1, glm::value_ptr(ambient));
        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 1, glm::value_ptr(diffuse));
        glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.specular"), 1, glm::value_ptr(specular));
    }

    void setDirection(const glm::vec3& dir) { direction = dir; }
    void setAmbient(const glm::vec3& amb) { ambient = amb; }
    void setDiffuse(const glm::vec3& diff) { diffuse = diff; }
    void setSpecular(const glm::vec3& spec) { specular = spec; }

    const glm::vec3& getDirection() const { return direction; }
    const glm::vec3& getAmbient() const { return ambient; }
    const glm::vec3& getDiffuse() const { return diffuse; }
    const glm::vec3& getSpecular() const { return specular; }

    glm::vec3 getPosition() const override {
        return Light::getPosition();
    }
    void updateDirectionFromRotation() override {
        const auto & transform = getComponent<Transform>();
        if (transform) {
            glm::mat4 rotationMatrix = transform->getRotationMatrix();

            direction = glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
           // std::cout << "Light Direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
        } else {
            std::cerr << "Transform is null, cannot update direction!" << std::endl;
        }
    }
};

#endif //NAV2SFM Core_DIR_LIGHT_H
