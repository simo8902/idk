//
// Created by Simeon on 7/14/2024.
//

#ifndef CYLINDER_H
#define CYLINDER_H

#include <vector>

#include "CylinderCollider.h"
#include "glad/glad.h"
#include "glm.hpp"
#include "GameObject.h"
#include "Mesh.h"

class Cylinder : public GameObject {
public:
    Cylinder(const std::string& name, float baseRadius, float topRadius, float height, int sectors);
    std::shared_ptr<GameObject> clone() const override;
    void Draw(const Shader& shader) override;

    // copy ctor
    Cylinder(const Cylinder& other) : GameObject(other) {
        if (other.m_collider) {
            m_collider = std::make_unique<CylinderCollider>(*other.m_collider);
        }
    }

    // copy assignment operator
    Cylinder& operator=(const Cylinder& other) {
        if (this == &other) return *this;
        GameObject::operator=(other);

        if (other.m_collider) {
            m_collider = std::make_unique<CylinderCollider>(*other.m_collider);
        } else {
            m_collider.reset();
        }

        return *this;
    }
private:
    float m_baseRadius;
    float m_topRadius;
    float m_height;
    int m_sectors;

    std::unique_ptr<CylinderCollider> m_collider;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    std::unique_ptr<Mesh > mesh;
};




#endif //NAV2SFM Core_CYLINDER_H
