//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef CUBE_H
#define CUBE_H

#include "BoxCollider.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "AssetManager.h"
#include "GameObject.h"

class Cube final : public GameObject {
public:
    explicit Cube(const std::string& name)
        : GameObject(name) {}

    void Initialize() {
        addComponents();
    }

    ~Cube() override = default;

    /*
    void debugPrint() const override {
        std::cout << "Cube: " << getName() << " (Virtual Asset)" << std::endl;
    }*/

    GameObjectType getType() const override {
        return GameObjectType::Cube;
    }

    void addComponents() {
        const auto & cubeMesh = std::make_shared<Mesh>("CubeMesh");
        const auto & cubeTransform = getComponent<Transform>();
        if (cubeTransform) {
            cubeTransform->setPosition(glm::vec3(0.0f, 1.5f, 0.0f));
        }

        cubeMesh->CreateMesh(MeshType::Cube);

        auto meshFilter = addComponent<MeshFilter>();
        meshFilter->setMesh(cubeMesh);
        auto meshRenderer = addComponent<MeshRenderer>(meshFilter);

        addComponent<BoxCollider>(
             cubeTransform ? cubeTransform->getPosition() : glm::vec3(0.0f),
             glm::vec3(-0.6f),
             glm::vec3(0.6f)
         );
    }
private:
    std::string name;
};

#endif
