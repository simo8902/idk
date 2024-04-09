//
// Created by Simeon on 4/8/2024.
//

#include "Scene.h"
#include "HierarchyManager.h"

std::shared_ptr<GameObject> HierarchyManager::selectedObject = nullptr;

void HierarchyManager::renderHierarchy(Scene* globalScene) {
    if (globalScene) {
        ImGui::Begin("Hierarchy");

        for (auto object : globalScene->getGameObjects()) {
            if (ImGui::Selectable(object->getName().c_str())) {
                selectedObject = object;
            }
        }

        ImGui::End();
    } else {
        ImGui::Text("No global scene available");
    }
}
