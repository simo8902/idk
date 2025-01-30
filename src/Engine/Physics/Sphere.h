//
// Created by Simeon on 7/15/2024.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "SphereCollider.h"
#include "Component.h"

class Sphere final : public Component {
public:
    explicit Sphere(const std::string& name): name(name){}
    ~Sphere() override {}

    const std::string & getName() const override {
        return name;
    }

    void addComponents() {
        const auto & sphereMesh = std::make_shared<Mesh>("Sphere");
        const auto & sphereTransform = addComponent<Transform>();
        const auto & meshFilter = addComponent<MeshFilter>("SPHERE_MESH");
        const auto & meshRenderer = addComponent<MeshRenderer>(meshFilter);
        const auto & sphereCollider = addComponent<SphereCollider>(sphereTransform->getPosition(), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        sphereMesh->CreateSphere(1.0f, 32, 16);
        meshFilter->setMesh(sphereMesh);
        sphereTransform->setPosition(glm::vec3(-2.50f, 1.65f, 0.0f));
    }

private:
    std::string name;
};

#endif
