//
// Created by Simeon on 4/17/2024.
//

#ifndef LUPUSFIRE_CORE_GIZMO_H
#define LUPUSFIRE_CORE_GIZMO_H

#include "../../Shader.h"
#include "../Component.h"

class Gizmo : public Component {
public:
    unsigned int VAO, VBO, EBO;

    Gizmo() {
        // The vertices represent a unit line along each axis
        float vertices[] = {
                0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
        };

        unsigned int indices[] = {
                0, 1,  // X axis
                0, 2,  // Y axis
                0, 3   // Z axis
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);  // Generate EBO

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  // Bind EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);  // Fill EBO with indices

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void draw(Shader &shader, const glm::mat4 &modelMatrix) {
        float scaleFactor = 15.0f;

        glm::mat4 scaledModelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor));

        glBindVertexArray(VAO);

        GLfloat originalLineWidth;
        glGetFloatv(GL_LINE_WIDTH, &originalLineWidth);
        glLineWidth(3.0f);

        shader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
        shader.setMat4("model", glm::scale(scaledModelMatrix, glm::vec3(0.1f, 0.01f, 0.01f)));
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);  // Draw X axis

        shader.setVec3("objectColor", glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", glm::scale(scaledModelMatrix, glm::vec3(0.01f, 0.1f, 0.01f)));
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(2 * sizeof(unsigned int)));  // Draw Y axis

        shader.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", glm::scale(scaledModelMatrix, glm::vec3(0.01f, 0.01f, 0.1f)));
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(4 * sizeof(unsigned int)));  // Draw Z axis

        glLineWidth(originalLineWidth);
    }
};

#endif //LUPUSFIRE_CORE_GIZMO_H
