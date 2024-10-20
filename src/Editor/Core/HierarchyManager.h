//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_HIERARCHYMANAGER_H
#define LUPUSFIRE_CORE_HIERARCHYMANAGER_H

#include <memory>
#include <vector>

class Renderer;
class Scene;
class LightManager;

class HierarchyManager {
public:
    HierarchyManager() = default;

    void renderHierarchy();

    // Setters for dependencies
    void setRenderer(Renderer* renderer);
    void setScene(Scene* scene);
    void setLightManager(const std::shared_ptr<LightManager>& lightManager);

private:
    Renderer* renderer = nullptr;
    Scene* scene = nullptr;
    std::shared_ptr<LightManager> lightManager; };


#endif //NAV2SFM Core_HIERARCHY_MANAGER_H
