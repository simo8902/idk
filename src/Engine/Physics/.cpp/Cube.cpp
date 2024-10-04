#include ".h/Cube.h"

Cube::Cube(const std::string& name)
    : GameObject(name) {
    std::vector<float> cubeVertices = Mesh::CreateCube();
    mesh = std::make_unique<Mesh>(cubeVertices, name);
}

Cube::~Cube() {}

void Cube::Draw(const Shader& shader) {
    if (mesh) {
        mesh->Draw(shader);
    } else {
        std::cerr << "[ERROR] Mesh is null for Cube: " << getName() << std::endl;
    }
}

/*
auto* material = getComponent<Material>();
if (material) {
  glm::vec3 color = material->getColor();
  shader.setVec3("objectColor", color);
}else{
  std::cerr << "Test";
}*/
