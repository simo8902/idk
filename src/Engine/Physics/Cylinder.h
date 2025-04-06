//
// Created by Simeon on 7/14/2024.
//

#ifndef CYLINDER_H
#define CYLINDER_H

#include "CylinderCollider.h"

class Cylinder final {
public:
    explicit Cylinder(const std::string &name){}
    ~Cylinder() = default;

    void addComponents() {
        /*
        const auto & cylinderMesh = std::make_shared<Mesh>("CylinderMesh");
        const auto & cylinderTransform = getComponent<Transform>();
        if (cylinderTransform) {
            cylinderTransform->setPosition(glm::vec3(2.0f, 1.5f, 0.0f));
        }

        cylinderMesh->CreateMesh(MeshType::Cylinder);

        const auto & meshFilter = addComponent<MeshFilter>();
        meshFilter->setMesh(cylinderMesh);

        const auto & meshRenderer = addComponent<MeshRenderer>(meshFilter);
        const auto & cylinderCollider = addComponent<CylinderCollider>(cylinderTransform->getPosition(), 2.0f, 0.5f);
        cylinderMesh->CreateCylinder(0.5f, 0.5f, 2.0f, 30);
*/
    }

private:
    std::string name;
    std::unique_ptr<CylinderCollider> m_collider;
};

#endif
