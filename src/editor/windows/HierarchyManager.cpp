//
// Created by Simeon on 4/8/2024.
//

#include "Scene.h"
#include "HierarchyManager.h"
#include "Renderer.h"

std::shared_ptr<GameObject> HierarchyManager::selectedObject = nullptr;

void HierarchyManager::renderHierarchy(Renderer* renderer) {
   if(renderer == nullptr)
       std::cerr << "Renderer is null\n";

    if (renderer) {
        ImGui::Begin("Hierarchy");

        for (auto object : renderer->getGameObjects()) {
            if (ImGui::Selectable(object->getName().c_str())) {
                selectedObject = object;
            }
        }

        ImGui::End();
    } else {
        ImGui::Text("No renderer available");
    }
}
