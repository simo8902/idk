//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef CUBE_H
#define CUBE_H

#include "BoxCollider.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Transform.h"

class Cube final : public Component {
public:
    explicit Cube(const std::string& name): name(name) {}
    ~Cube() override{}

    const std::string & getName() const override {
        return name;
    }

    void addComponents() {
        const auto & cubeMesh = std::make_shared<Mesh>("Cube");
        const auto & cubeTransform = addComponent<Transform>();
        const auto & meshFilter = addComponent<MeshFilter>("CUBE_MESH");
        const auto & meshRenderer = addComponent<MeshRenderer>(meshFilter);
        const auto & cubeCollider = addComponent<BoxCollider>(cubeTransform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));
        meshFilter->setMesh(std::make_shared<Mesh>(Mesh::CreateCube(), "Cube"));
        cubeTransform->setPosition(glm::vec3(4.0f, 1.5f, 0.0f));
    }

private:
    std::string name;
    std::unique_ptr<BoxCollider> m_collider;
};

#endif
