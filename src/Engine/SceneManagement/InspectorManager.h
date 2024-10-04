//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_INSPECTORMANAGER_H
#define LUPUSFIRE_CORE_INSPECTORMANAGER_H

#include <memory>
#include <string>
#include "imgui.h"
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "GameObject.h"
#include "Light.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "SelectionManager.h"

class InspectorManager {
public:
    InspectorManager() = default;

    void renderInspector();

private:
    void renderGameObjectInspector(const std::shared_ptr<GameObject>& gameObject);
    void renderLightInspector(const std::shared_ptr<Light>& light);
    void renderCameraInspector(const std::shared_ptr<Camera>& camera);
    void renderMeshInspector(const std::shared_ptr<Mesh>& mesh);
    void renderMaterialInspector(const std::shared_ptr<Material>& material);

    void renderTransformComponent(const std::shared_ptr<Transform>& transform);
};

#endif //LUPUSFIRE_CORE_INSPECTORMANAGER_H
