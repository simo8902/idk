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
#include "Component.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "Transform.h"
#include <algorithm>

#include "Selectable.h"

enum class GameObjectType {
    Cube,
    Capsule,
    Light,
    Cylinder,
    Camera,
    Sphere
};

class GameObject : public std::enable_shared_from_this<GameObject>  {
public:
    explicit GameObject(const std::string& name)
        : m_name(name) {
        std::cerr << "GO CREATED WITH NAME(): " << name << std::endl;
        addComponent<Transform>();

    }

    virtual ~GameObject() {}

    virtual const std::string & getName() {
        return m_name;
    }

    virtual void setName(const std::string& newName) {
        m_name = newName;
    }

    virtual GameObjectType getType() const = 0;

    template <typename T, typename... Args>
    std::shared_ptr<T> addComponent(Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        m_components.push_back(component);
        return component;
    }

    template <typename T>
    bool hasComponent() const
    {
        for (const auto& component : m_components) {
            if (std::dynamic_pointer_cast<T>(component)) {
                return true;
            }
        }
        return false;
    }

    const std::vector<std::shared_ptr<Component>>& getComponents() const {
        return m_components;
    }

    // Retrieve all components of a specific type
    template <typename T>
    std::vector<std::shared_ptr<T>> getComponentsOfType() const {
        std::vector<std::shared_ptr<T>> result;
        for (const auto& component : m_components) {
            if (auto comp = std::dynamic_pointer_cast<T>(component)) {
                result.push_back(comp);
            }
        }
        return result;
    }

    template <typename T>
    std::shared_ptr<T> getComponent() {
        for (const auto& component : m_components) {
            if (auto comp = std::dynamic_pointer_cast<T>(component)) {
                return comp;
            }
        }
        return nullptr;
    }

    void update() {
        for (const auto& component : m_components) {
         //   component->update();
        }
    }

    virtual void render() {
        // Can implement rendering logic here
        std::cerr << "Rendering GameObject: " << m_name << std::endl;
    }

    glm::vec3 color;

    void addChild(const std::shared_ptr<GameObject>& child) {
        if (child && std::find(children.begin(), children.end(), child) == children.end()) {
            children.push_back(child);
            child->parent = shared_from_this();
        }
    }

    void removeChild(const std::shared_ptr<GameObject>& child) {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            children.erase(it);
            child->parent.reset();
        }
    }

    const std::vector<std::shared_ptr<GameObject>>& getChildren() const {
        return children;
    }

    bool operator==(const GameObject& other) const {
        return this == &other;
    }

private:
    std::string m_name;
    std::vector<std::shared_ptr<GameObject>> children;
    std::weak_ptr<GameObject> parent;

protected:
    std::vector<std::shared_ptr<Component>> m_components;
};


#endif
