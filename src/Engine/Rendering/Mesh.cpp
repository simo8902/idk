//
// Created by Simeon on 9/27/2024.
//

#include "Mesh.h"

#include <ext/scalar_constants.hpp>

Mesh::Mesh(const std::vector<float>& vertices, const std::string& name)
    : vertices(vertices), name(name) {
    SetupMesh();
}

Mesh::Mesh(const std::string &name)
    : name(name), VAO(0), VBO(0), EBO(0) {}

Mesh::~Mesh() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Mesh::SetupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if (!indices.empty()) {
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);

    // Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Element Buffer
    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Vertex Attributes
    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) {
    shader.Use();
    glBindVertexArray(VAO);
    if (!indices.empty()) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));
    }

    glBindVertexArray(0);
}

void Mesh::CreateSphere(float radius, int stacks, int sectors) {
    vertices.clear();
    indices.clear();

    float x, y, z, nx, ny, nz;
    float stackStep = glm::pi<float>() / stacks;
    float sectorStep = 2 * glm::pi<float>() / sectors;
    float stackAngle, sectorAngle;

    // Generate vertices
    for(int i = 0; i <= stacks; ++i) {
        stackAngle = glm::pi<float>() / 2 - i * stackStep; // from pi/2 to -pi/2
        y = radius * sin(stackAngle);
        float xy = radius * cos(stackAngle); // r * cos(theta)

        for(int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep; // from 0 to 2pi

            x = xy * cos(sectorAngle);
            z = xy * sin(sectorAngle);
            nx = x / radius;
            ny = y / radius;
            nz = z / radius;

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            // Normal
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    // Generate indices
    int k1, k2;
    for(int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1); // beginning of current stack
        k2 = k1 + sectors + 1;  // beginning of next stack

        for(int j = 0; j < sectors; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            if(i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stacks-1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    // Setup the mesh with the new vertices and indices
    SetupMesh();
}

void Mesh::CreateCylinder(float m_baseRadius, float m_topRadius, float m_height, int m_sectors) {
    vertices.clear();
    indices.clear();

    float sectorStep = 2 * glm::pi<float>() / m_sectors;
    float sectorAngle;

    // Generate vertices for the side
    for (int i = 0; i <= m_sectors; ++i) {
        sectorAngle = i * sectorStep;
        float x = cos(sectorAngle);
        float z = sin(sectorAngle);

        // Position (base)
        vertices.push_back(x * m_baseRadius);
        vertices.push_back(-m_height / 2.0f);
        vertices.push_back(z * m_baseRadius);
        // Normal
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);

        // Position (top)
        vertices.push_back(x * m_topRadius);
        vertices.push_back(m_height / 2.0f);
        vertices.push_back(z * m_topRadius);
        // Normal
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
    }

    // Indices for the side
    for (int i = 0; i < m_sectors; ++i) {
        int k1 = 2 * i;
        int k2 = 2 * i + 1;
        int k3 = 2 * (i + 1);
        int k4 = 2 * (i + 1) + 1;

        // First triangle
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k3);

        // Second triangle
        indices.push_back(k2);
        indices.push_back(k4);
        indices.push_back(k3);
    }

    // Base center vertex
    unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size() / 6);
    vertices.push_back(0.0f);
    vertices.push_back(-m_height / 2.0f);
    vertices.push_back(0.0f);
    // Normal
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);
    vertices.push_back(0.0f);

    // Top center vertex
    unsigned int topCenterIndex = static_cast<unsigned int>(vertices.size() / 6);
    vertices.push_back(0.0f);
    vertices.push_back(m_height / 2.0f);
    vertices.push_back(0.0f);
    // Normal
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);

    // Indices for the base
    for (int i = 0; i < m_sectors; ++i) {
        int k1 = 2 * i;
        int k2 = 2 * (i + 1);
        indices.push_back(baseCenterIndex);
        indices.push_back(k2);
        indices.push_back(k1);
    }

    // Indices for the top
    for (int i = 0; i < m_sectors; ++i) {
        int k1 = 2 * i + 1;
        int k2 = 2 * (i + 1) + 1;
        indices.push_back(topCenterIndex);
        indices.push_back(k1);
        indices.push_back(k2);
    }

    // Setup the mesh with the new vertices and indices
    SetupMesh();
}
void Mesh::CreateCapsule(float radius, float height, int resolution) {
    vertices.clear();
    indices.clear();

    for (int i = 0; i <= resolution; ++i) {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(resolution);
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);

        vertices.push_back(x);               // Position X
        vertices.push_back(0.0f);            // Position Y
        vertices.push_back(z);               // Position Z
        vertices.push_back(0.0f);            // Normal X
        vertices.push_back(-1.0f);           // Normal Y
        vertices.push_back(0.0f);            // Normal Z

        vertices.push_back(x);               // Position X
        vertices.push_back(height);          // Position Y
        vertices.push_back(z);               // Position Z
        vertices.push_back(0.0f);            // Normal X
        vertices.push_back(1.0f);            // Normal Y
        vertices.push_back(0.0f);            // Normal Z
    }

    for (int i = 0; i <= resolution; ++i) {
        float phi = glm::pi<float>() * float(i) / float(resolution) / 2.0f;

        for (int j = 0; j <= resolution; ++j) {
            float theta = 2.0f * glm::pi<float>() * float(j) / float(resolution);
            float x = radius * cosf(theta) * sinf(phi);
            float y = radius * cosf(phi);
            float z = radius * sinf(theta) * sinf(phi);

            vertices.push_back(x);              // Position X
            vertices.push_back(y + height);     // Position Y (shift by height)
            vertices.push_back(z);              // Position Z
            vertices.push_back(x);              // Normal X
            vertices.push_back(y);              // Normal Y
            vertices.push_back(z);              // Normal Z

            vertices.push_back(x);              // Position X
            vertices.push_back(-y);             // Position Y (bottom hemisphere)
            vertices.push_back(z);              // Position Z
            vertices.push_back(x);              // Normal X
            vertices.push_back(-y);             // Normal Y
            vertices.push_back(z);              // Normal Z
        }
    }

    SetupMesh();
}

std::vector<float> Mesh::CreateCube()
{
    return{
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
}