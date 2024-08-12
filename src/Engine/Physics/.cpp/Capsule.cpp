//
// Created by Simeon on 5/9/2024.
//

#include ".h/Capsule.h"

size_t Capsule::memoryUsage = 0;

Capsule::Capsule(const std::string &name) : GameObject(name), VAO(0), VBO(0) {
    Capsule::memoryUsage += sizeof(Capsule);

    SetupMesh(1.0f, 2.0f, 512);
}

Capsule::~Capsule() {
    Capsule::memoryUsage -= sizeof(Capsule);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);

}

void Capsule::SetupMesh(float radius, float height, int resolution) {
    vertices.clear();

    // Cylinder vertices
    for (int i = 0; i <= resolution; ++i) {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(resolution);
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);

        // Bottom circle
        vertices.push_back({x, 0.0f, z});
        // Top circle
        vertices.push_back({x, height, z});
    }

    // Hemispheres vertices
    for (int i = 0; i <= resolution; ++i) {
        float phi = glm::pi<float>() * float(i) / float(resolution) / 2.0f;

        for (int j = 0; j <= resolution; ++j) {
            float theta = 2.0f * glm::pi<float>() * float(j) / float(resolution);
            float x = radius * cosf(theta) * sinf(phi);
            float y = radius * cosf(phi);
            float z = radius * sinf(theta) * sinf(phi);

            // Top hemisphere
            vertices.push_back({x, y + height, z});
            // Bottom hemisphere
            vertices.push_back({x, -y, z});
        }
    }

    // Setup OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glBindVertexArray(0);

    memoryUsage += vertices.size() * sizeof(Vertex);
    memoryUsage += sizeof(GLuint); // VAO
    memoryUsage += sizeof(GLuint); // VBO
}

void Capsule::Draw(const Shader &shader) {
    shader.Use();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}
