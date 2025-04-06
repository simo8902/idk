//
// Created by SIMEON on 4/5/2025.
//

#ifndef ICOMPONENTMANAGER_H
#define ICOMPONENTMANAGER_H

#include <cstdint>
#include <memory>
#include <typeindex>
#include "Component.h"

using EntityID = uint32_t;

class IComponentManager {
public:
    virtual ~IComponentManager() = default;

    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    T& addComponent(EntityID id, Args&&... args) {
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        addComponentImpl(id, std::type_index(typeid(T)), comp);
        return *comp;
    }

    template<typename T, typename F,
        typename = std::enable_if_t<!std::is_constructible_v<T, F> && std::is_invocable_v<F, T&>>>
    T& addComponent(EntityID id, F&& initializer) {
        static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
        auto comp = std::make_shared<T>();
        std::forward<F>(initializer)(*comp);
        addComponentImpl(id, std::type_index(typeid(T)), comp);
        return *comp;
    }

    template<typename T>
    T* getComponent(EntityID id) const {
        return static_cast<T*>(getComponentImpl(id, std::type_index(typeid(T))));
    }

    template<typename T>
    void removeComponent(EntityID id) {
        removeComponentImpl(id, std::type_index(typeid(T)));
    }
    virtual std::unordered_map<std::type_index, std::shared_ptr<Component>>
      getAllComponentsForEntity(EntityID id) const = 0;

protected:
    virtual void* getComponentImpl(EntityID id, std::type_index type) const = 0;
    virtual void addComponentImpl(EntityID id, std::type_index type, std::shared_ptr<Component> comp) = 0;
    virtual void removeComponentImpl(EntityID id, std::type_index type) = 0;
};

#endif //ICOMPONENTMANAGER_H