//
// Created by Simeon on 4/7/2024.
//

#ifndef CORE_COMPONENT_H
#define CORE_COMPONENT_H

#include <vector>
#include <memory>
#include <iostream>

class Component {
public:
    explicit Component(const std::string& name)
    : m_Name(name)
    {
        // std::cerr << "Component Name: " << m_Name << std::endl;
    }

    virtual ~Component()= default;

    virtual const std::string& getName() const {
        return m_Name;
    }
    virtual std::string getType() const {
        // return typeid(*this).name(); //RTTI
        return "BASE::Component";
    }

    virtual void setName(const std::string& newName) {
        m_Name = newName;
    }

    const std::vector<std::shared_ptr<Component>>& getComponents() const {
        return m_components;
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

private:
    std::vector<std::shared_ptr<Component>> m_components;
    std::string m_Name;
};


#endif
