//
// Created by Simeon on 4/10/2024.
//

#ifndef LUPUSFIRE_CORE_GIZMO_H
#define LUPUSFIRE_CORE_GIZMO_H

#include <vector>
#include "glm.hpp"
#include "src/components/Transform.h"
#include "src/Shader.h"

class Gizmo {
public:
    Gizmo() {
        setupGeometry();
    }

    ~Gizmo() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void render(const Shader& shader, const Transform& objectTransform) {
        shader.Use();

        glm::mat4 gizmoModel = objectTransform.getModelMatrix();

        shader.setMat4("model", gizmoModel);

        glBindVertexArray(vao);

        // Draw X-axis (red)
        shader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
        glDrawArrays(GL_LINES, 0, 2);


        // Draw Y-axis (green)
        shader.setVec3("objectColor", glm::vec3(0.0f, 1.0f, 0.0f));
        glDrawArrays(GL_LINES, 2, 2);

        // Draw Z-axis (blue)
        shader.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 1.0f));
        glDrawArrays(GL_LINES, 4, 2);

        glBindVertexArray(0);
    }

private:
    void setupGeometry() {
        std::vector<GLfloat> vertices = {
                // X-axis
                0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,

                // Y-axis
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,

                // Z-axis
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0); // Unbind VAO
    }

    GLuint vao, vbo;
};

#endif //LUPUSFIRE_CORE_GIZMO_H
