//
// Created by Simeon on 9/22/2024.
//

#ifndef LIGHT_H
#define LIGHT_H

#include "Transform.h"
#include "Component.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <typeinfo>

class Light {
public:
    Light(std::string name)
        : name(std::move(name)) {
        transform = std::make_shared<Transform>();
        addComponent(transform);
    }

    virtual ~Light() = default;

    virtual void setUniforms(unsigned int shaderProgram) const = 0;
    virtual void updateDirectionFromRotation() {}

    std::shared_ptr<Transform> transform;

    bool hasTransform() const {
        return transform != nullptr;
    }

    virtual glm::vec3 getPosition() const {
        return position;
    }

    std::string getName() const { return name; }

    template<typename T>
    std::shared_ptr<T> getComponent() {
        auto it = components.find(typeid(T).name());
        if (it != components.end()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        return nullptr; // Component not found
    }

    template <typename T>
    void addComponent(std::shared_ptr<T> component) {
        components[typeid(T).name()] = std::static_pointer_cast<Component>(component);
    }
    std::string name;

private:
    std::unordered_map<std::string, std::shared_ptr<Component>> components;

protected:
    glm::vec3 position;
};
#endif //NAV2SFM Core_LIGHT_H
