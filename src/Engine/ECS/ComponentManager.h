//
// Created by SIMEON on 4/5/2025.
//

#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "IComponentManager.h"
#include <unordered_map>

class ComponentManager : public IComponentManager {
public:
    std::unordered_map<std::type_index, std::shared_ptr<Component>>
        getAllComponentsForEntity(EntityID id) const override {
        std::unordered_map<std::type_index, std::shared_ptr<Component>> result;

        for (const auto& [type, entityMap] : m_components) {
            auto it = entityMap.find(id);
            if (it != entityMap.end()) {
                result[type] = it->second;
            }
        }

        return result;
    }

protected:
    void* getComponentImpl(EntityID id, std::type_index type) const override {
        auto it = m_components.find(type);
        if (it != m_components.end()) {
            auto compIt = it->second.find(id);
            if (compIt != it->second.end())
                return compIt->second.get();
        }
        return nullptr;
    }

    void addComponentImpl(EntityID id, std::type_index type, std::shared_ptr<Component> comp) override {
        m_components[type][id] = comp;
    }

    void removeComponentImpl(EntityID id, std::type_index type) override {
        auto it = m_components.find(type);
        if (it != m_components.end())
            it->second.erase(id);
    }


private:
    std::unordered_map<std::type_index, std::unordered_map<EntityID, std::shared_ptr<Component>>> m_components;
};
#endif //COMPONENTMANAGER_H
