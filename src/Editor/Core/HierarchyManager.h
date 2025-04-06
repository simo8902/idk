//
// Created by Simeon on 4/8/2024.
//

#ifndef CORE_HIERARCHYMANAGER_H
#define CORE_HIERARCHYMANAGER_H

#include <imgui.h>
#include <memory>
#include <vector>

#include "GameObject.h"
#include "LightManager.h"
#include "Scene.h"
#include "SelectionManager.h"
#include "Renderer.h"

namespace IDK
{
    class Renderer;
}
class Scene;
class Entity;

class HierarchyManager {
public:
    HierarchyManager(const HierarchyManager&) = delete;
    HierarchyManager& operator=(const HierarchyManager&) = delete;

    static HierarchyManager& getInstance() {
        static HierarchyManager instance;
        return instance;
    }

    void initialize(IDK::Renderer* renderer, std::shared_ptr<IDK::Scene> scene);
    void renderHierarchyContent();

    /*
    void renderHierarchy() const;

    // Setters for dependencies
    void setRenderer(Renderer* renderer);
    void setScene(const std::shared_ptr<Scene> & scene);
    void setLightManager(const std::shared_ptr<LightManager>& lightManager);
    void renderEntityTree(const std::shared_ptr<Entity>& entity, size_t parentId) const;

    const std::vector<std::shared_ptr<Entity>>& getComponents() const {
        if (scene) {
            return scene->getComponents();
        }
        return emptyComponents;
    }

    void selectEntity(const std::shared_ptr<Entity>& entity) {
        selectedEntity = entity;
    }

    std::shared_ptr<Entity> getSelectedEntity() const {
        return selectedEntity;
    }

    void clearSelection() {
        selectedEntity.reset();
    }
*/
    void selectEntity(const std::shared_ptr<Entity>& entity);
    std::shared_ptr<Entity> getSelectedEntity() const;
    void clearSelection();

    const std::vector<std::shared_ptr<Entity>>& getEntities() const;

private:

    std::shared_ptr<Entity> m_selectedEntity;

    /*
    HierarchyManager() = default;
    std::shared_ptr<Scene> scene = nullptr;
    std::shared_ptr<Entity> selectedEntity = nullptr;
    mutable std::vector<std::shared_ptr<Entity>> emptyComponents;*/
    static const std::vector<std::shared_ptr<Entity>> m_emptyVector;
    HierarchyManager() = default;

    // Rendering helpers
    void renderEntityRow(const std::shared_ptr<Entity>& entity, size_t index);
    void handleSelectionClear();

    // Dependencies
    IDK::Renderer* m_renderer = nullptr;
    std::shared_ptr<IDK::Scene> m_scene;
};


#endif
