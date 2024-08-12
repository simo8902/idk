//
// Created by Simeon on 7/15/2024.
//

#include ".h/Sphere.h"
#include <cmath>
#include <iostream>

size_t Sphere::memoryUsage = 0;

Sphere::Sphere(const std::string& name)
    : GameObject(name), VAO(0), VBO(0) {
    SetupMesh(1.0f, 1024);

    Sphere::memoryUsage += sizeof(Sphere);
}

Sphere::~Sphere() {
    Sphere::memoryUsage -= sizeof(Sphere);

    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }
}

void Sphere::SetupMesh(float radius, int resolution) {
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

    // Setup OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SphereVertex), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)0);

    glBindVertexArray(0);

    memoryUsage += vertices.size() * sizeof(SphereVertex);
    memoryUsage += sizeof(GLuint); // VAO
    memoryUsage += sizeof(GLuint); // VBO
}

void Sphere::Draw(const Shader& shader) {
    shader.Use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}