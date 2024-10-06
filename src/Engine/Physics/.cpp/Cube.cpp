#include ".h/Cube.h"

Cube::Cube(const std::string& name)
    : GameObject(name) {
    std::vector<float> cubeVertices = Mesh::CreateCube();
    mesh = std::make_unique<Mesh>(cubeVertices, name);
}

Cube::~Cube() {}

void Cube::Draw(const Shader& shader) {}