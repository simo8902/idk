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
        if (!isInitialized()) {
            std::cerr << "DirectionalLight is not initialized. Cannot set uniforms." << std::endl;
            return;
        }
        GLint dirLoc = glGetUniformLocation(shaderProgram, "dirLight.direction");
        GLint ambLoc = glGetUniformLocation(shaderProgram, "dirLight.ambient");
        GLint diffLoc = glGetUniformLocation(shaderProgram, "dirLight.diffuse");
        GLint specLoc = glGetUniformLocation(shaderProgram, "dirLight.specular");

        if (dirLoc == -1) std::cerr << "Uniform 'dirLight.direction' not found!" << std::endl;
        if (ambLoc == -1) std::cerr << "Uniform 'dirLight.ambient' not found!" << std::endl;
        if (diffLoc == -1) std::cerr << "Uniform 'dirLight.diffuse' not found!" << std::endl;
        if (specLoc == -1) std::cerr << "Uniform 'dirLight.specular' not found!" << std::endl;

        if (dirLoc != -1) glUniform3fv(dirLoc, 1, glm::value_ptr(direction));
        if (ambLoc != -1) glUniform3fv(ambLoc, 1, glm::value_ptr(ambient));
        if (diffLoc != -1) glUniform3fv(diffLoc, 1, glm::value_ptr(diffuse));
        if (specLoc != -1) glUniform3fv(specLoc, 1, glm::value_ptr(specular));


        /*
        std::cout << "Light Direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
        std::cout << "Light Ambient: " << ambient.r << ", " << ambient.g << ", " << ambient.b << std::endl;
        std::cout << "Light Diffuse: " << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << std::endl;
        std::cout << "Light Specular: " << specular.r << ", " << specular.g << ", " << specular.b << std::endl;*/
    }

    void testDirectionalLightInitialization() {
        try {
            auto dirLight = std::make_shared<DirectionalLight>(
                "Test Light",
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.1f, 0.1f, 0.1f),
                glm::vec3(1.0f, 1.0f, 1.0f),
                glm::vec3(1.0f, 1.0f, 1.0f)
            );
            assert(dirLight->isInitialized());
            std::cout << "DirectionalLight initialization test passed." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Test failed: " << e.what() << std::endl;
        }
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

#endif
