//
// Created by Simeon on 7/15/2024.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "SphereCollider.h"

class Sphere final : public GameObject {
public:
    explicit Sphere(const std::string& name)
        : GameObject(name) {}

    ~Sphere() override {}

    GameObjectType getType() const override {
        return GameObjectType::Sphere;
    }

    void addComponents() {
        const auto & sphereMesh = std::make_shared<Mesh>("SphereMesh");
        const auto & sphereTransform = getComponent<Transform>();
        if (sphereTransform) {
            sphereTransform->setPosition(glm::vec3(4.25f, 1.5f, 0.0f));
        }

        sphereMesh->CreateMesh(MeshType::Sphere);

        auto meshFilter = addComponent<MeshFilter>();
        meshFilter->setMesh(sphereMesh);

        auto meshRenderer = addComponent<MeshRenderer>(meshFilter);
        const auto & sphereCollider = addComponent<SphereCollider>(sphereTransform->getPosition(), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    }

private:
    std::string name;
};

#endif
