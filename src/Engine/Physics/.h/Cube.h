//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include "glad/glad.h"
#include "Shader.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "MeshRenderer.h"

class Cube : public GameObject {
public:
    Cube(const std::string& name);
    ~Cube() override;

    // copy ctor
    Cube(const Cube& other) : GameObject(other) {
        if (other.m_collider) {
            m_collider = std::make_unique<BoxCollider>(*other.m_collider);
        }
    }

    // copy assignment operator
    Cube& operator=(const Cube& other) {
        if (this == &other) return *this;
        GameObject::operator=(other);

        if (other.m_collider) {
            m_collider = std::make_unique<BoxCollider>(*other.m_collider);
        } else {
            m_collider.reset();
        }

        return *this;
    }

    void Draw(const Shader& shader) override;

    std::shared_ptr<GameObject> clone() const override {
        auto newCube = std::make_shared<Cube>(*this);

        newCube->m_name = this->m_name;
        std::cout << "Copied name: " << newCube->m_name << "\n";

        newCube->m_components.clear();
        for (const auto& component : this->m_components) {
            newCube->m_components.push_back(component->clone());
        }
        return newCube;
    }
private:
    std::unique_ptr<BoxCollider> m_collider;
    std::unique_ptr<Mesh> mesh;
};

#endif //NAV2SFM Core_CUBE_H
