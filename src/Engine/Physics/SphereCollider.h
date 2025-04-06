//
// Created by Simeon on 7/15/2024.
//

#ifndef SPHERECOLLIDER_H
#define SPHERECOLLIDER_H

#include <memory>
#include <glad/glad.h>
#include <vector>
#include "Shader.h"

#include "Collider.h"

struct SphereColVertex {
    float x, y, z;
};

class SphereCollider final : public Collider {
public:
    SphereCollider(const glm::vec3 & position, float radius, glm::vec3 center, const std::string& name = "SphereCollider")
        : Collider(name), m_position(position), m_radius(radius), m_center(center)
    {
        sphereCollider = this;
        updateModelMatrix();
        SetupMesh(m_radius, 128);
    }

    ~SphereCollider() override {}

    void updateModelMatrix() {
        m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
     }
    bool intersectsRay(const Ray &ray, const glm::mat4 &transformMatrix, float distance) override {
        glm::vec3 worldCenter = glm::vec3(transformMatrix * glm::vec4(m_center, 1.0f));

        // Extract scaling factors from the transform matrix
        float scaleX = glm::length(glm::vec3(transformMatrix[0]));
        float scaleY = glm::length(glm::vec3(transformMatrix[1]));
        float scaleZ = glm::length(glm::vec3(transformMatrix[2]));

        // Compute the maximum scaling factor to scale the radius
        float maxScale = std::max(scaleX, std::max(scaleY, scaleZ));
        float worldRadius = m_radius * maxScale;

        // Calculate the vector from the ray origin to the sphere center
        glm::vec3 oc = ray.m_origin - worldCenter;

        // Calculate coefficients of the quadratic equation
        float a = glm::dot(ray.m_direction, ray.m_direction);
        float b = 2.0f * glm::dot(oc, ray.m_direction);
        float c = glm::dot(oc, oc) - worldRadius * worldRadius;

        // Calculate the discriminant
        float discriminant = b * b - 4.0f * a * c;

        // Check if the discriminant is non-negative (i.e., ray intersects sphere)
        if (discriminant < 0.0f) return false;

        // Calculate the two possible intersection points
        float sqrtDiscriminant = sqrt(discriminant);
        float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t1 = (-b + sqrtDiscriminant) / (2.0f * a);

        // Return true if either of the intersection points is within the distance
        if ((t0 >= 0.0f && t0 <= distance) || (t1 >= 0.0f && t1 <= distance)) {
            return true;
        }

        return false;

    }

    void SetupMesh(float radius, int resolution) {
        vertices.clear();

        for (int i = 0; i <= resolution; ++i) {
            float phi = glm::pi<float>() * float(i) / float(resolution);

            for (int j = 0; j <= resolution; ++j) {
                float theta = 2.0f * glm::pi<float>() * float(j) / float(resolution);
                float x = radius * sinf(phi) * cosf(theta);
                float y = radius * cosf(phi);
                float z = radius * sinf(phi) * sinf(theta);

                vertices.push_back({x, y, z});
            }
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SphereColVertex), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereColVertex), (void*)0);

        glBindVertexArray(0);
    }

    void Draw(IDK::Graphics::Shader &wireframe) override {
        if(sphereCollider) {
            wireframe.Use();
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
            glBindVertexArray(0);
        }
    }

    glm::vec3 m_position;
    float m_radius;

private:
    const SphereCollider* sphereCollider;
    glm::mat4 m_modelMatrix;
    std::vector<SphereColVertex> vertices;
    GLuint VAO, VBO;
    glm::vec3 m_center;
};

#endif