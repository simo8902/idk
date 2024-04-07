//
// Created by Simeon on 4/5/2024.
//

#ifndef LUPUSFIRE_CORE_GAMEOBJECT_H
#define LUPUSFIRE_CORE_GAMEOBJECT_H

#include <iostream>
#include <vector>
#include <memory>
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "Shader.h"
#include "components/Component.h"
#include "components/BoundingBox.h"
#include "components/Transform.h"

class GameObject {
public:
    GameObject(const std::string& name) : m_name(name) {
        std::cout << "GameObject created: " << this->getName() << std::endl;
    }

    // Component Management
    template <typename T>
    T* addComponent() {
        T* newComponent = new T();
        m_components.emplace_back(std::unique_ptr<Component>(newComponent));
        return newComponent;
    }

    template <typename T>
    T* getComponent() {
        for (auto& component : m_components) {
            if (T* comp = dynamic_cast<T*>(component.get())) {
                return comp;
            }
        }
        return nullptr;
    }


    void update() {
        // Update the game object...

        // Update the bounding box
        BoundingBox* boundingBox = getComponent<BoundingBox>();
        if (boundingBox) {
            Transform* transform = getComponent<Transform>();
            if (transform) {
                glm::mat4 modelMatrix = transform->getModelMatrix();

                // Transform the bounding box
                boundingBox->min = glm::vec3(modelMatrix * glm::vec4(boundingBox->min, 1.0f));
                boundingBox->max = glm::vec3(modelMatrix * glm::vec4(boundingBox->max, 1.0f));
            }
        }
    }

    void setColor(const glm::vec3& color) {
        this->color = color;
    }

    glm::vec3 getColor() const {
        return color;
    }


    const std::string& getName() const { return m_name; }
    virtual void Draw(Shader& shader) = 0;
    bool isSelected = false;

private:
    std::string m_name;
    std::vector<std::unique_ptr<Component>> m_components;
    glm::vec3 color;

};

#endif //LUPUSFIRE_CORE_GAMEOBJECT_H
