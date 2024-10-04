//
// Created by Simeon on 9/27/2024.
//

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "glad/glad.h"
#include "glm.hpp"
#include "Shader.h"

class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::string& name);
    ~Mesh();

    const std::vector<float>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const {
        return indices;
    }

    void Draw(const Shader& shader);

    static std::vector<float> CreateCube();
    const std::string& getName() const { return name; }
    void SetupMesh();
    bool hasMesh() const { return !vertices.empty(); }

private:
    GLuint VAO, VBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::string name;
};



#endif //MESH_H
