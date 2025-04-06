//
// Created by SIMEON on 3/13/2025.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <string>

#include "Common.h"
#include "Component.h"
#include "IComponentManager.h"
#include "Selectable.h"

class Entity : public Selectable{
public:
    Entity(EntityID id, const std::string& name, EntityType type, IComponentManager* componentManager)
        : m_id(id), m_name(name), m_type(type), m_componentManager(componentManager) {}

    EntityID getID() const noexcept { return m_id; }
    std::string getName() const noexcept override { return m_name; }
    EntityType getType() const noexcept { return m_type; }

    template<typename T>
    bool hasComponent() const {
        return m_componentManager && m_componentManager->getComponent<T>(m_id) != nullptr;
    }

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        if (!m_componentManager) throw std::runtime_error("Entity is not associated with a ComponentManager");
        return m_componentManager->addComponent<T>(m_id, std::forward<Args>(args)...);
    }

    template<typename T>
    T* getComponent() const {
        if (!m_componentManager) return nullptr;
        return m_componentManager->getComponent<T>(m_id);
    }

    template<typename T>
    void removeComponent() {
        if (!m_componentManager) throw std::runtime_error("Entity is not associated with a ComponentManager");
        m_componentManager->removeComponent<T>(m_id);
    }
    void addChild(const std::shared_ptr<Entity>& child) {
        m_children.push_back(child);
    }

    const std::vector<std::shared_ptr<Entity>>& getChildren() const {
        return m_children;
    }

    std::unordered_map<std::type_index, std::shared_ptr<Component>> getAllComponents() const {
        if (!m_componentManager) {
            throw std::runtime_error("Entity is not associated with a ComponentManager");
        }
        return m_componentManager->getAllComponentsForEntity(m_id);
    }

    void select() override {
        if (m_selected) return;

        m_selected = true;

        // Notify all selectable components
        const auto& components = getAllComponents();
        for (const auto& [type, component] : components) {
            if (auto selectable = std::dynamic_pointer_cast<Selectable>(component)) {
                selectable->select();
            }
        }

        // Custom selection logic if needed
        onSelected();
    }

    void deselect() override {
        if (!m_selected) return;

        m_selected = false;

        // Notify all selectable components
        const auto& components = getAllComponents();
        for (const auto& [type, component] : components) {
            if (auto selectable = std::dynamic_pointer_cast<Selectable>(component)) {
                selectable->deselect();
            }
        }

        // Custom deselection logic if needed
        onDeselected();
    }


    bool isSelected() const { return m_selected; }

protected:
    virtual void onSelected() {}
    virtual void onDeselected() {}
private:
    EntityID m_id;
    std::string m_name;
    EntityType m_type;
    IComponentManager* m_componentManager;
    std::vector<std::shared_ptr<Entity>> m_children;
    bool m_selected = false;
};


#endif //ENTITY_H
