//
// Created by Simeon on 7/14/2024.
//

#ifndef CYLINDER_H
#define CYLINDER_H

#include "CylinderCollider.h"

class Cylinder final : public Component {
public:
    explicit Cylinder(const std::string &name): name(name){}
    ~Cylinder() override = default;

    const std::string & getName() const override {
        return name;
    }

    void addComponents() {
        const auto & cylinderMesh = std::make_shared<Mesh>("Cylinder");
        const auto & cylinderTransform = addComponent<Transform>();

        const auto & meshFilter = addComponent<MeshFilter>("CYLINDER_MESH");
        const auto & meshRenderer = addComponent<MeshRenderer>(meshFilter);
        const auto & cylinderCollider = addComponent<CylinderCollider>(cylinderTransform->getPosition(), 2.0f, 0.5f);
        cylinderMesh->CreateCylinder(0.5f, 0.5f, 2.0f, 30);
        meshFilter->setMesh(cylinderMesh);
        cylinderTransform->setPosition(glm::vec3(2.00f, 1.65f, 0.0f));
    }

private:
    std::string name;
    std::unique_ptr<CylinderCollider> m_collider;
};

#endif
