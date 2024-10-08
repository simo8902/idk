//
// Created by Simeon on 9/27/2024.
//

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "glad/glad.h"
#include "glm.hpp"
#include "Shader.h"
#include <filesystem>
#include "AssetItem.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};
class Mesh : public AssetItem {
public:
    ~Mesh();
    Mesh(const std::vector<float>& vertices, const std::string& name);
    Mesh(const std::string& name);

    const std::vector<float> & getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

    void Draw(const Shader& shader);

    void CreateSphere(float radius, int stacks, int sectors);
    const std::string& getName() const { return name; }
    void SetupMesh();
    bool hasMesh() const { return !vertices.empty(); }
    void CreateCapsule(float radius, float height, int resolution);
    void CreateCylinder(float m_baseRadius, float m_topRadius, float m_height, int m_sectors);
    static std::vector<float> CreateCube();

    std::string name;

private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};



#endif //MESH_H
