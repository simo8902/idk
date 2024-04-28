//
// Created by Simeon on 4/8/2024.
//

#include "Scene.h"
#include "HierarchyManager.h"
#include "Renderer.h"

std::shared_ptr<Camera> HierarchyManager::selectedCamera = nullptr;

void HierarchyManager::renderHierarchy(Renderer* renderer) {
    ImGui::Begin("Hierarchy");
    if(renderer == nullptr)
       std::cerr << "Renderer is null\n";

    if (renderer) {

        /*
        for (auto object : renderer->getGameObjects()) {
            if (ImGui::Selectable(object->getName().c_str())) {
                renderer->selectedObject = object;
                selectedCamera = nullptr;
            }
        }

        std::shared_ptr<Camera> camera = renderer->getCamera();
        if (camera != nullptr) {
            if (ImGui::Selectable(camera->getName().c_str())) {
                selectedCamera = camera;
                renderer->selectedObject = nullptr;
            }
        }
*/
    } else {
        ImGui::Text("No renderer available");
    }
    ImGui::End();
}
