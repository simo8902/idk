//
// Created by SIMEON on 4/5/2025.
//

#ifndef REGISTRY_H
#define REGISTRY_H

#include "ComponentManager.h"
#include "Entity.h"
#include <unordered_map>
#include <memory>

class Registry {
public:
    static Registry& instance() {
        static Registry instance;
        return instance;
    }

    std::shared_ptr<Entity> createEntity(const std::string& name, EntityType type = EntityType::Unknown) {
        EntityID id = m_nextID++;
        auto entity = std::make_shared<Entity>(id, name, type, m_componentManager.get());
        m_entities[id] = entity;

        return entity;
    }

    void destroyEntity(EntityID id) {
        m_entities.erase(id);
    }

    IComponentManager* getComponentManager() const {
        return m_componentManager.get();
    }

private:
    Registry() : m_nextID(0), m_componentManager(std::make_unique<ComponentManager>()) {}
    ~Registry() = default;
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;

    std::unordered_map<EntityID, std::shared_ptr<Entity>> m_entities;
    std::unique_ptr<IComponentManager> m_componentManager;
    EntityID m_nextID;
};

#endif //REGISTRY_H
