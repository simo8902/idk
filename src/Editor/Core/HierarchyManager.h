//
// Created by Simeon on 4/8/2024.
//

#ifndef CORE_HIERARCHYMANAGER_H
#define CORE_HIERARCHYMANAGER_H

#include <memory>
#include <vector>

class Renderer;
class Scene;
class LightManager;

class HierarchyManager {
public:
    HierarchyManager() = default;

    void renderHierarchy() const;

    // Setters for dependencies
    void setRenderer(Renderer* renderer);
    void setScene(const std::shared_ptr<Scene> & scene);
    void setLightManager(const std::shared_ptr<LightManager>& lightManager);

private:
    Renderer* renderer = nullptr;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<LightManager> lightManager; };


#endif
