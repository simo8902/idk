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

class GameObject {
public:
    GameObject(const std::string& name) : m_name(name) {
        std::cout << "GameObject created: " << this->getName() << std::endl;
    }

    // Object Management
    template <typename T>
    std::shared_ptr<T> addObject(const std::string& name){
        std::shared_ptr<T> newObject = std::make_shared<T>(name);
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
        return newComponent;
    }

    template <typename T>
    T* getComponent() {
        for (std::unique_ptr<Component> & component : m_components) {
            if (T* comp = dynamic_cast<T*>(component.get())) {
                return comp;
            }
        }
        std::cerr << "Debug: No component of type " << typeid(T).name() << " found in object " << getName() << std::endl;

        return nullptr;
    }


    const std::string& getName() const { return m_name; }

    virtual void Draw(const Shader& shader) = 0;
    void DebugDraw(const Shader& wireframe);

    void setColor(glm::vec3 m_color) {
        this->color = m_color;
    }

    glm::vec3 getColor() const {
        return color;
    }

    glm::vec3 color;
private:
    std::string m_name;
    std::vector<std::unique_ptr<Component>> m_components;
    unsigned int VAO, VBO, EBO;
};

#endif //LUPUSFIRE_CORE_GAMEOBJECT_H
