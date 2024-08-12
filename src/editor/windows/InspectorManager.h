//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_INSPECTORMANAGER_H
#define LUPUSFIRE_CORE_INSPECTORMANAGER_H

#include "imgui.h"
#include "../../GameObject.h"
#include "../../Camera.h"

class Renderer;

class InspectorManager {
public:
    void renderInspector(const std::shared_ptr<GameObject>& selectedObject);
    void renderInspector(const std::shared_ptr<Camera>& camera);

};


#endif //LUPUSFIRE_CORE_INSPECTORMANAGER_H
