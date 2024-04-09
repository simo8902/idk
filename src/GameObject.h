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

    // Object Management
    template <typename T>
    std::shared_ptr<T> addObject(const std::string& name){
        std::shared_ptr<T> newObject = std::make_shared<T>(name);

        std::cerr << "Obj added successfully\n";

        return newObject;
    }

    // Component Management
    template <typename T>
    T* addComponent() {
        T* newComponent = new T();
        m_components.emplace_back(std::unique_ptr<Component>(newComponent));

        return newComponent;
    }

    template <typename T, typename... Args>
    T* addComponent(Args&&... args) {
        T* newComponent = new T(std::forward<Args>(args)...);
        m_components.emplace_back(std::unique_ptr<Component>(newComponent));
        std::cout << "Debug: Added component of type " << typeid(T).name() << " at address " << newComponent << " to object " << getName() << std::endl;

        return newComponent;
    }

    template <typename T>
    T* getComponent() {
        for (auto& component : m_components) {
            if (T* comp = dynamic_cast<T*>(component.get())) {
                std::cout << "Debug: Retrieved component of type " << typeid(T).name() << " at address " << comp << " from object " << getName() << std::endl;
                return comp;
            }
        }
        std::cout << "Debug: No component of type " << typeid(T).name() << " found in object " << getName() << std::endl;

        return nullptr;
    }


    const std::string& getName() const { return m_name; }
    virtual void Draw(Shader& shader) = 0;
    bool isSelected = false;

    void DebugDraw(Shader& shader);

    void setColor(glm::vec3 m_color) {
        this->color = m_color;
    }

    glm::vec3 getColor() const {
        return color;
    }
private:
    std::string m_name;
    std::vector<std::unique_ptr<Component>> m_components;
    glm::vec3 color;
    unsigned int VAO, VBO, EBO;
    bool useBoxColliderColor;

};

#endif //LUPUSFIRE_CORE_GAMEOBJECT_H
