//
// Created by simeon on 5/9/2024.
//

#ifndef CAPSULE_H
#define CAPSULE_H

#include "Mesh.h"
#include "CapsuleCollider.h"
#include <string>
#include <memory>

#include "Selectable.h"

class Capsule final : public GameObject, public Selectable  {
public:
    explicit Capsule(const std::string& name):GameObject(name){}
    ~Capsule() override {}

    std::string getName() const override {
        return name;
    }

    void select() override {
        std::cout << "Capsule selected: " << name << std::endl;
    }

    void deselect() override {
        std::cout << "Capsule deselected: " << name << std::endl;
    }

    GameObjectType getType() const override {
        return GameObjectType::Capsule;
    }
    void addComponents() {
        const auto & capsuleMesh = std::make_shared<Mesh>("Capsule");
        const auto & capsuleTransform = getComponent<Transform>();
        if (capsuleTransform) {
            capsuleTransform->setPosition(glm::vec3(-2.5f, 1.5f, 0.0f));
        }
        capsuleMesh->CreateMesh(MeshType::Capsule);

        const auto & meshFilter = addComponent<MeshFilter>();
        meshFilter->setMesh(capsuleMesh);

        const auto & meshRenderer = addComponent<MeshRenderer>(meshFilter);
        const auto & collider = addComponent<CapsuleCollider>(capsuleTransform->getPosition(), 1.0f, 2.0f);
    }

private:
    std::string name;
};

#endif //CAPSULE_H
