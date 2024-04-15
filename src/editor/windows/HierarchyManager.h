//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_HIERARCHYMANAGER_H
#define LUPUSFIRE_CORE_HIERARCHYMANAGER_H

#include "imgui.h"

class Renderer;

class HierarchyManager {
public:
    static std::shared_ptr<GameObject> selectedObject;
    void renderHierarchy(Renderer* renderer);
private:
};


#endif //LUPUSFIRE_CORE_HIERARCHYMANAGER_H
