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

enum class MeshType {
    Cube,
    Capsule,
    Sphere,
    Cylinder
};

class Mesh final : public AssetItem {
public:
    Mesh(const Mesh &other, const std::string &newName);
    ~Mesh() override;
    explicit Mesh(const std::vector<float>& vertices, const std::string& name);
    explicit Mesh(const std::string& name);

    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

    void Draw(const Shader& shader) const;

    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;

    void CreateMesh(MeshType type);

    const std::string& getName() const { return name; }
    void SetupMesh();
    bool hasMesh() const { return !vertices.empty(); }

    void CreateSphere(float radius, int stacks, int sectors);
    void CreateCapsule(const float& radius, const float& height);
    void CreateCylinder(float m_baseRadius, float m_topRadius, float m_height, int m_sectors);
    void CreateCube();

    std::string name;

private:
    GLuint VAO{}, VBO{}, EBO{};
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};



#endif //MESH_H
