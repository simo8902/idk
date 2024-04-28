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

    virtual ~GameObject(){}
    virtual std::shared_ptr<GameObject> clone() const = 0;

    virtual void Draw(const Shader& shader) = 0;

    // Copy constructor
    GameObject(const GameObject& other)
            : m_name(other.m_name) {
     //   std::cout << "GameObject copied: " << this->getName() << std::endl;


        for (const auto& component : other.m_components) {
            m_components.push_back(component);  // std::shared_ptr can be copied
        }
    }

    const std::vector<std::shared_ptr<Component>>& getComponents() const {
        return m_components;
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
        for (std::shared_ptr<Component> & component : m_components) {
            if (T* comp = dynamic_cast<T*>(component.get())) {
                return comp;
            }
        }
        return nullptr;
    }


    virtual std::string getName() const {
        return m_name;
    }


    void DebugDraw(const Shader& wireframe);

    glm::vec3 color;
    std::string m_name;

private:
    std::vector<std::shared_ptr<Component>> m_components;
    unsigned int VAO, VBO, EBO;
};

#endif //LUPUSFIRE_CORE_GAMEOBJECT_H
