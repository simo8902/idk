#include "Triangle.h"

#include "libs/glad/include/glad/glad.h"


Triangle::Triangle() {
    // Vertices of a triangle
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Bottom-left
        0.5f, -0.5f, 0.0f,  // Bottom-right
        0.0f,  0.5f, 0.0f   // Top
    };

    // Setup mesh
    SetupMesh();

    // Bind VBO and buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

Triangle::~Triangle() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Triangle::Draw() {
    // Draw the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Triangle::SetupMesh() {
    // Generate vertex array and vertex buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO and set vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);
}