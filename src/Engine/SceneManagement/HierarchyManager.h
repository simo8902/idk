//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_HIERARCHYMANAGER_H
#define LUPUSFIRE_CORE_HIERARCHYMANAGER_H

#include "imgui.h"
#include "GameObject.h"
#include "InspectorManager.h"

class Renderer;
class Scene;

class HierarchyManager {
public:
    static std::shared_ptr<Camera> selectedCamera;

    void renderHierarchy(Renderer* renderer, Scene* scene);
 };


#endif //LUPUSFIRE_CORE_HIERARCHYMANAGER_H
