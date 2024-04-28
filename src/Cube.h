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

    void Draw(const Shader& shader) override;

    virtual std::shared_ptr<GameObject> clone() const override {
        return std::make_shared<Cube>(*this);
    }

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;

    void SetupMesh();
};




#endif //CUBE_H
