//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include "glad/glad.h"
#include "Shader.h"

#include "GameObject.h"

class Cube : public GameObject {
public:
    Cube(const std::string& name);
    ~Cube();

    void Draw(Shader& shader) override;

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;

    void SetupMesh();
};




#endif //CUBE_H
