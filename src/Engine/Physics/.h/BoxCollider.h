//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_BOXCOLLIDER_H
#define LUPUSFIRE_CORE_BOXCOLLIDER_H

#include "Collider.h"
#include "Ray.h"
#include "gtx/string_cast.hpp"
#include "glad/glad.h"

class BoxCollider : public Collider {
public:
    BoxCollider(const glm::vec3& position, const glm::vec3& min, const glm::vec3& max)
            : m_position(position), m_worldMin(min), m_worldMax(max)  {
        collider = this;
        updateModelMatrix();
    }

    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getMin() const { return m_worldMin; }
    const glm::vec3& getMax() const { return m_worldMax; }


    std::unique_ptr<Component> clone() const override {
        return std::make_unique<BoxCollider>(*this);
    }

    void setPosition(const glm::vec3& pos) {
        m_position = pos;
        updateModelMatrix();
    }
    void updateModelMatrix() {
        m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
    }

    const glm::mat4& getModelMatrix() const {
        return m_modelMatrix;
    }

    glm::vec3 m_worldMin;
    glm::vec3 m_worldMax;
    glm::vec3 m_position;
    glm::mat4 m_modelMatrix;

    bool intersectsRay(const Ray& ray, const glm::mat4& transformMatrix, float distance) override {
        glm::mat4 invTransform = glm::inverse(transformMatrix);

        glm::vec3 rayOriginLocal = glm::vec3(invTransform * glm::vec4(ray.getOrigin(), 1.0f));
        glm::vec3 rayDirectionLocal = glm::normalize(glm::vec3(invTransform * glm::vec4(ray.getDirection(), 0.0f))); // Normalize here!

        glm::vec3 rayDirectionWorld = glm::vec3(transformMatrix * glm::vec4(rayDirectionLocal, 0.0f));
        if (glm::dot(rayDirectionWorld, m_worldMax - ray.getOrigin()) < 0) {
            std::cerr << "Error: Ray is not pointing towards the bounding box!" << std::endl;
            return false;
        }

        // Check if the ray direction is normalized
        if (glm::length(rayDirectionLocal) < 0.99f || glm::length(rayDirectionLocal) > 1.01f) {
            std::cerr << "Error: Ray direction is not normalized!" << std::endl;
            return false;
        }

        // Check bounding box dimensions
        if (m_worldMin.x >= m_worldMax.x || m_worldMin.y >= m_worldMax.y || m_worldMin.z >= m_worldMax.z) {
            std::cerr << "Error: Invalid bounding box dimensions!" << std::endl;
            return false;
        }

        if (rayDirectionLocal == glm::vec3(0.0f)) {
            std::cerr << "Error: Division by zero!" << std::endl;
            return false;
        }

        glm::vec3 invDir = 1.0f / rayDirectionLocal;
        glm::vec3 t0s = (m_worldMin - rayOriginLocal) * invDir;
        glm::vec3 t1s = (m_worldMax - rayOriginLocal) * invDir;

        glm::vec3 tmin = glm::min(t0s, t1s);
        glm::vec3 tmax = glm::max(t0s, t1s);

        float tMin = std::max(tmin.x, std::max(tmin.y, tmin.z));
        float tMax = std::min(tmax.x, std::min(tmax.y, tmax.z));

        if (tMax < 0 || tMin > tMax) {
            return false;
        }

        return true;
    }

    void Draw(Shader& wireframe) override  {
        if (collider) {
            glm::vec3 min = collider->getMin();
            glm::vec3 max = collider->getMax();

            float vertices[] = {
                    min.x, min.y, min.z,
                    max.x, min.y, min.z,
                    max.x, max.y, min.z,
                    min.x, max.y, min.z,
                    min.x, min.y, max.z,
                    max.x, min.y, max.z,
                    max.x, max.y, max.z,
                    min.x, max.y, max.z
            };

            unsigned int indices[] = {
                    0, 1, 1, 2, 2, 3, 3, 0, // Front face
                    4, 5, 5, 6, 6, 7, 7, 4, // Back face
                    0, 4, 1, 5, 2, 6, 3, 7  // Sides
            };
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);

            // std::cerr << "Debug: " << getName() << " has a BoxCollider component." << std::endl;

            glBindVertexArray(VAO);

            glm::vec3 wireframeColor = glm::vec3(0.0f, 1.0f, 0.0f);
            wireframe.setVec3("m_wireframeColor", wireframeColor);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
            glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

private:
    BoxCollider* collider;
    GLuint VAO, VBO, EBO;
};

#endif //LUPUSFIRE_CORE_BOXCOLLIDER_H
