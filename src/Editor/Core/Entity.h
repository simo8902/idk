//
// Created by SIMEON on 3/13/2025.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <memory>
#include "GameObject.h"
#include "Selectable.h"

class Entity final : public std::enable_shared_from_this<Entity>, public Selectable{
public:
    explicit Entity(const std::shared_ptr<GameObject>& gameObject) : gameObject(gameObject) {
        // std::cerr << "Entities: " << gameObject->getName()
        //     << " Type of: " << gameObject->getType() << std::endl;
    }

    std::string getName() const override {
        if (gameObject) {
            return gameObject->getName();
        }
        return "Unnamed Entity";
    }

    void setName(const std::string& newName) {
        if (gameObject) {
            gameObject->setName(newName);
        }
    }

    void select() override {
        if (gameObject) {
            isSelected = true;
            std::cout << "Entity selected: " << getName() << std::endl;
        }
    }

    void deselect() override {
        if (gameObject) {
            isSelected = false;
            std::cout << "Entity deselected: " << getName() << std::endl;
        }
    }

    std::string getType() const {
        return gameObject ? gameObjectTypeToString(gameObject->getType()) : "Unknown";
    }

    std::string getScript() const {
        return "Placeholder";
    }

    bool isVisible() const {
        return visible;
    }

    void setVisible(bool isVisible) {
        visible = isVisible;
    }

    void addChild(std::shared_ptr<Entity> child) {
        child->parent = shared_from_this();
        children.push_back(child);
    }

    std::vector<std::shared_ptr<Entity>>& getChildren() {
        return children;
    }

    std::shared_ptr<Entity> getParent() const {
        return parent.lock();
    }
    /*
    std::shared_ptr<GameObject> getComponentBase() {
        return component;
    }*/

    std::shared_ptr<GameObject> getComponent() const {
        return gameObject;
    }

    template<typename T>
    std::shared_ptr<T> getComponent() const {
        if (gameObject) {
            auto gameObjectComponent = gameObject->getComponent<T>();
            if (gameObjectComponent) return gameObjectComponent;
        }

        /*
        // Then check the Entity's components
        for (const auto& comp : gameObject) {
            if (auto casted = std::dynamic_pointer_cast<T>(comp)) {
                return casted;
            }
        }*/
        return nullptr;
    }

    const std::vector<std::shared_ptr<Component>>& getComponents() const {
        return gameObject->getComponents();
    }
    /*
    void addComponent(std::shared_ptr<Component> component) {
        components.push_back(component);
    }*/

    template<typename T>
    bool hasComponent() const {
        return gameObject->hasComponent<T>();
    }
private:
    static std::string gameObjectTypeToString(GameObjectType type) {
        switch (type) {
            case GameObjectType::Cube: return "Cube";
            case GameObjectType::Capsule: return "Capsule";
            case GameObjectType::Light: return "Light";
            case GameObjectType::Sphere: return "Sphere";
            case GameObjectType::Cylinder: return "Cylinder";
            case GameObjectType::Camera: return "Camera";
            default: return "Unknown";
        }
    }

    bool visible = true;
    std::weak_ptr<Entity> parent;
    std::vector<std::shared_ptr<Entity>> children;
    bool isSelected = false;

    std::shared_ptr<GameObject> gameObject;
   // std::shared_ptr<GameObject> component;
    std::vector<std::shared_ptr<Component>> components;
};

#endif //ENTITY_H
