//
// Created by Simeon on 7/14/2024.
//

#ifndef CYLINDERCOLLIDER_H
#define CYLINDERCOLLIDER_H

#include "Collider.h"
#include "Ray.h"
#include "glad/glad.h"
#include "memory"
#include <vector>

#define M_PI 3.14159265358979323846

class CylinderCollider final : public Collider {
public:
    CylinderCollider(const glm::vec3& pos, const float h, const float r, const std::string& name = "CylinderdCollider")
        : Collider(name), position(pos), height(h), radius(r)
    {
        generateVertices();
        setupMesh();
        updateModelMatrix();
    }

    void Draw(IDK::Graphics::Shader& wireframe) override {
        wireframe.Use();

        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);
    }

    bool intersectsRay(const Ray& ray, const glm::mat4& transformMatrix, float distance) override {
        glm::mat4 invTransform = glm::inverse(transformMatrix);
        glm::vec3 localRayOrigin = glm::vec3(invTransform * glm::vec4(ray.m_origin, 1.0f));
        glm::vec3 localRayDirection = glm::normalize(glm::vec3(invTransform * glm::vec4(ray.m_direction, 0.0f)));

        // Check intersection with infinite cylinder
        float a = localRayDirection.x * localRayDirection.x + localRayDirection.z * localRayDirection.z;
        float b = 2.0f * (localRayOrigin.x * localRayDirection.x + localRayOrigin.z * localRayDirection.z);
        float c = localRayOrigin.x * localRayOrigin.x + localRayOrigin.z * localRayOrigin.z - radius * radius;

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) {
            return false;
        }

        discriminant = sqrt(discriminant);
        float t1 = (-b - discriminant) / (2.0f * a);
        float t2 = (-b + discriminant) / (2.0f * a);

        if (t1 > t2) std::swap(t1, t2);

        // Check if the intersection points are within the height of the cylinder
        float y1 = localRayOrigin.y + t1 * localRayDirection.y;
        float y2 = localRayOrigin.y + t2 * localRayDirection.y;

        bool intersectSide = false;
        if ((y1 >= -height / 2.0f && y1 <= height / 2.0f) ||
            (y2 >= -height / 2.0f && y2 <= height / 2.0f)) {
            intersectSide = true;
        }

        float tCap1 = (-height / 2.0f - localRayOrigin.y) / localRayDirection.y;
        float tCap2 = (height / 2.0f - localRayOrigin.y) / localRayDirection.y;

        float cap1x = localRayOrigin.x + tCap1 * localRayDirection.x;
        float cap1z = localRayOrigin.z + tCap1 * localRayDirection.z;
        bool intersectCap1 = (tCap1 >= 0) && (cap1x * cap1x + cap1z * cap1z <= radius * radius);

        float cap2x = localRayOrigin.x + tCap2 * localRayDirection.x;
        float cap2z = localRayOrigin.z + tCap2 * localRayDirection.z;
        bool intersectCap2 = (tCap2 >= 0) && (cap2x * cap2x + cap2z * cap2z <= radius * radius);

        distance = std::numeric_limits<float>::max();
        if (intersectSide) {
            if (y1 >= -height / 2.0f && y1 <= height / 2.0f) distance = t1;
            if (y2 >= -height / 2.0f && y2 <= height / 2.0f && t2 < distance) distance = t2;
        }
        if (intersectCap1 && tCap1 < distance) distance = tCap1;
        if (intersectCap2 && tCap2 < distance) distance = tCap2;

        return distance < std::numeric_limits<float>::max();
    }


    void updateModelMatrix() {
        m_modelMatrix = glm::translate(glm::mat4(1.0f), position);
    }

    void setPosition(const glm::vec3& pos) {
        position = pos;
        updateModelMatrix();
    }
    float height;
    float radius;
private:
    glm::vec3 position;
    glm::mat4 m_modelMatrix;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    void generateVertices() {
        float sectorStep = 2 * M_PI / 30;
        float sectorAngle;

        // Generate vertices
        for (int i = 0; i <= 30; ++i) {
            sectorAngle = i * sectorStep;
            float x = cos(sectorAngle);
            float z = sin(sectorAngle);

            // Base vertex
            vertices.push_back(glm::vec3(x * radius, -height / 2.0f, z * radius));

            // Top vertex
            vertices.push_back(glm::vec3(x * radius, height / 2.0f, z * radius));
        }

        // Generate indices for the cylinder sides
        for (int i = 0; i < 30; ++i) {
            indices.push_back(2 * i);
            indices.push_back(2 * i + 1);
            indices.push_back(2 * i + 2);

            indices.push_back(2 * i + 1);
            indices.push_back(2 * i + 3);
            indices.push_back(2 * i + 2);
        }

        // Add center vertices for base and top
        vertices.push_back(glm::vec3(0.0f, -height / 2.0f, 0.0f));
        vertices.push_back(glm::vec3(0.0f, height / 2.0f, 0.0f));
        unsigned int baseCenter = vertices.size() - 2;
        unsigned int topCenter = vertices.size() - 1;

        // Generate indices for the cylinder caps
        for (int i = 0; i < 30; ++i) {
            indices.push_back(baseCenter);
            indices.push_back(2 * i);
            indices.push_back(2 * i + 2);

            indices.push_back(topCenter);
            indices.push_back(2 * i + 1);
            indices.push_back(2 * i + 3);
        }
    }

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
};

#endif