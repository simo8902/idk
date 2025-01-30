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

class GameObject {
public:
    GameObject() {
    }

    GameObject(const std::string& name)
        : m_name(name) {
    }

    virtual ~GameObject() {}

    virtual void Draw(const Shader& shader) {
        if (!isBeingRendered) {
            isBeingRendered = true;

            std::cout << "[GameObject] Base Draw called." << std::endl;

            isBeingRendered = false;
        } else {
            std::cerr << "[ERROR] Draw called recursively for object." << std::endl;
        }
    }

    void printComponents() {}

    virtual std::string getName() const {
        return m_name;
    }

    virtual void setName(const std::string& newName) {
        m_name = newName;
    }

    virtual void onMeshCleared() {
        std::cerr << "[DEBUG] Mesh cleared from GameObject: " << m_name << "\n";
    }

    void DebugDraw(const Shader& wireframe);

    glm::vec3 color;
    std::string m_name;


protected:
    bool isBeingRendered = false;
    std::mutex m_mutex;

};


#endif
