//
// Created by Simeon on 4/5/2024.
//

#include "GameObject.h"
#include "components/colliders/BoxCollider.h"

void GameObject::DebugDraw(const Shader& wireframe) {
    BoxCollider* collider = getComponent<BoxCollider>();

    if (collider) {
        // Fetch required data
        glm::vec3 position = collider->getPosition();
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // std::cerr << "Debug: " << getName() << " has a BoxCollider component." << std::endl;

        glBindVertexArray(VAO);

        //TODO: implement the color of the wireframe
        glm::vec3 wireframeColor = glm::vec3(0.0f, 1.0f, 0.0f);
        wireframe.setVec3("m_wireframeColor", wireframeColor);


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
