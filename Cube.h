//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include "libs/glad/include/glad/glad.h"
#include "Shader.h"

class Cube {
public:
    Cube();
    void Draw(Shader& shader);
    void setColor(glm::vec3 color) {
        this->color = color;
    }
    glm::vec3 getColor() const {
        return color;
    }
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    glm::vec3 color;

    void SetupMesh();
};




#endif //CUBE_H
