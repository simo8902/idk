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
#include <cxxabi.h>
#include <unordered_map>
#include <typeindex>
#include <memory>


class GameObject {
public:
    GameObject(const std::string& name) : m_name(name) {
        memoryUsage += sizeof(GameObject);
    }

    virtual ~GameObject() {
        memoryUsage -= sizeof(GameObject);
    }
    virtual std::shared_ptr<GameObject> clone() const = 0;

    virtual void Draw(const Shader& shader) = 0;

    const std::vector<std::shared_ptr<Component>>& getComponents() const {
        return m_components;
    }

    // Object Management
    template <typename T>
    std::shared_ptr<T> addObject(const std::string& name) {
        std::shared_ptr<T> newObject = std::make_shared<T>(name);
        return newObject;
    }

    // Component Management
    template <typename T>
    std::shared_ptr<T> addComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        auto newComponent = std::make_shared<T>();
        m_components.emplace_back(newComponent);
        return newComponent;
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        auto newComponent = std::make_shared<T>(std::forward<Args>(args)...);
        m_components.emplace_back(newComponent);
        return newComponent;
    }

    // Non-const
    template <typename T>
    std::shared_ptr<T> getComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        for (const auto& component : m_components) {
            auto comp = std::dynamic_pointer_cast<T>(component);
            if (comp) {
                return comp;
            }
        }
        return nullptr;
    }

    // Const
    template <typename T>
    std::shared_ptr<const T> getComponent() const {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        for (const auto& component : m_components) {
            auto comp = std::dynamic_pointer_cast<const T>(component);
            if (comp) {
                return comp;
            }
        }
        return nullptr;
    }

    void printComponents() {
        for (const auto& component : m_components) {
            int status;
            char * demangled = abi::__cxa_demangle(typeid(*component).name(), 0, 0, &status);
            if(status == 0) {
                std::cout << demangled << std::endl;
                free(demangled);
            } else {
                std::cout << "Demangling failed" << std::endl;
            }
        }
    }

    virtual std::string getName() const {
        return m_name;
    }

    virtual void setName(const std::string& newName) {
        m_name = newName;
    }

    virtual void onMeshCleared() {
        std::cerr << "[DEBUG] Mesh cleared from GameObject: " << m_name << "\n";
    }

    static size_t GetMemoryUsage() {
        return memoryUsage;
    }

    void DebugDraw(const Shader& wireframe);

    glm::vec3 color;
    std::string m_name;
    std::vector<std::shared_ptr<Component>> m_components;
protected:
    static size_t memoryUsage;
};


#endif //NAV2SFM Core_GAMEOBJECT_H
