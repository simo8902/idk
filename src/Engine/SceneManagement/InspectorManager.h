//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_INSPECTORMANAGER_H
#define LUPUSFIRE_CORE_INSPECTORMANAGER_H

#include "GameObject.h"
#include "Camera.h"
#include "LightManager.h"

class InspectorManager {
public:
    static void renderInspector(const std::shared_ptr<GameObject>& selectedObject);
    static void renderInspector(const std::shared_ptr<Camera>& camera);
    static void renderInspector(const std::shared_ptr<Light>& light);

};


#endif //LUPUSFIRE_CORE_INSPECTORMANAGER_H
