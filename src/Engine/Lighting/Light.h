//
// Created by Simeon on 9/22/2024.
//

#ifndef LIGHT_H
#define LIGHT_H

#include "Transform.h"
#include "../ECS/Component.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace IDK::Graphics
{
    class Light : public std::enable_shared_from_this<Light> {
    public:
        Light(const std::string& name)
            : name(name) {
            std::cerr << "LIGHT()" << std::endl;

            transform = std::make_shared<Transform>();
            addComponent(transform);
        }

        virtual ~Light() = default;

        virtual bool isInitialized() const {
            return transform != nullptr && !name.empty();
        }
        void setPosition(const glm::vec3& position) {
            if (transform) {
                transform->setPosition(position);
            }
        }

        virtual void setUniforms(unsigned int shaderProgram) const = 0;
        virtual void updateDirectionFromRotation() {}

        std::shared_ptr<Transform> transform;

        bool hasTransform() const {
            return transform != nullptr;
        }

        virtual glm::vec3 getPosition() const {
            if (transform)
                return transform->getPosition();
            return glm::vec3(0.0f);
        }

        std::string getName() const { return name; }

        // Non-const
        template<typename T>
        std::shared_ptr<T> getComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
            auto it = components.find(std::type_index(typeid(T)));
            if (it != components.end()) {
                return std::dynamic_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

        // Const
        template<typename T>
        std::shared_ptr<const T> getComponent() const {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
            auto it = components.find(std::type_index(typeid(T)));
            if (it != components.end()) {
                return std::dynamic_pointer_cast<const T>(it->second);
            }
            return nullptr;
        }

        template <typename T>
        void addComponent(const std::shared_ptr<T>& component) {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
            components[std::type_index(typeid(T))] = component;
        }
        std::string name;

    protected:
        std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    };
}
#endif
