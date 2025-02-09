//
// Created by simeon on 5/9/2024.
//

#ifndef CAPSULE_H
#define CAPSULE_H

#include "Mesh.h"
#include "CapsuleCollider.h"
#include <string>
#include <memory>

class Capsule final : public Component {
public:
    explicit Capsule(const std::string& name) : name(name) {}
    ~Capsule() override {}

    const std::string & getName() const override {
        return name;
    }

    void addComponents() {
        const auto & capsuleMesh = std::make_shared<Mesh>("Capsule");
        const auto & capsuleTransform = addComponent<Transform>();

        const auto & meshFilter = addComponent<MeshFilter>("CAPSULE_MESH");

        const auto & meshRenderer = addComponent<MeshRenderer>(meshFilter);
        capsuleMesh->CreateCapsule(1.0f, 2.0f);
        meshFilter->setMesh(capsuleMesh);
        capsuleTransform->setPosition(glm::vec3(0.00f, 1.65f, 0.0f));

        const auto & collider = addComponent<CapsuleCollider>(capsuleTransform->getPosition(), 1.0f, 2.0f);


    }

private:
    std::string name;
    std::vector<std::weak_ptr<Component>> components;
};

#endif //CAPSULE_H
