//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_HIERARCHYMANAGER_H
#define LUPUSFIRE_CORE_HIERARCHYMANAGER_H

#include "InspectorManager.h"
#include "LightManager.h"

class Renderer;
class Scene;

class HierarchyManager {
public:
    static std::shared_ptr<Camera> selectedCamera;
    static std::shared_ptr<Light> selectedLight;

    void renderHierarchy(Renderer* renderer, Scene* scene, std::shared_ptr<LightManager> lightManager);
 };


#endif //NAV2SFM Core_HIERARCHY_MANAGER_H
