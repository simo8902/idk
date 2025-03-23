//
// Created by SIMEON on 2/22/2025.
//

#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

enum RendererType {
    FORWARD_RENDERER,
    DEFERRED_RENDERER
};

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Camera.h"
#include "InspectorManager.h"
#include "ProjectExplorer.h"
#include "HierarchyManager.h"
#include "ImGuizmo.h"

class RendererManager {
public:
    RendererManager(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Camera>& camera, GLFWwindow* window);
    virtual ~RendererManager();

};

#endif //RENDERERMANAGER_H
