//
// Created by Simeon on 7/14/2024.
//

#include ".h/Cylinder.h"
#include <cmath>

Cylinder::Cylinder(const std::string& name, float baseRadius, float topRadius, float height, int sectors)
    : GameObject(name), m_baseRadius(baseRadius), m_topRadius(topRadius), m_height(height), m_sectors(sectors)
{
    generateVertices();
    setupMesh();
}

std::shared_ptr<GameObject> Cylinder::clone() const {
    return std::make_shared<Cylinder>(*this);
}

void Cylinder::Draw(const Shader& shader) {

    shader.Use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Cylinder::generateVertices() {
    float sectorStep = 2 * M_PI / m_sectors;
    float sectorAngle;

    for (int i = 0; i <= m_sectors; ++i) {
        sectorAngle = i * sectorStep;
        float x = cos(sectorAngle);
        float z = sin(sectorAngle);

        // Base vertex
        vertices.push_back(glm::vec3(x * m_baseRadius, -m_height / 2.0f, z * m_baseRadius));

        // Top vertex
        vertices.push_back(glm::vec3(x * m_topRadius, m_height / 2.0f, z * m_topRadius));
    }

    // Indices for the cylinder sides
    for (int i = 0; i < m_sectors; ++i) {
        indices.push_back(2 * i);
        indices.push_back(2 * i + 1);
        indices.push_back(2 * i + 2);

        indices.push_back(2 * i + 1);
        indices.push_back(2 * i + 3);
        indices.push_back(2 * i + 2);
    }

    // Indices for the cylinder caps
    unsigned int baseCenter = vertices.size();
    unsigned int topCenter = vertices.size() + 1;

    for (int i = 0; i < m_sectors; ++i) {
        indices.push_back(baseCenter);
        indices.push_back(2 * i);
        indices.push_back(2 * i + 2);

        indices.push_back(topCenter);
        indices.push_back(2 * i + 1);
        indices.push_back(2 * i + 3);
    }
}

void Cylinder::setupMesh() {
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
