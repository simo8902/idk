//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_HIERARCHYMANAGER_H
#define LUPUSFIRE_CORE_HIERARCHYMANAGER_H

#include "imgui.h"
#include "Renderer.h"

class HierarchyManager {
public:
    static std::shared_ptr<GameObject> selectedObject;
    void renderHierarchy();
private:
    Renderer* renderer;
};


#endif //LUPUSFIRE_CORE_HIERARCHYMANAGER_H
