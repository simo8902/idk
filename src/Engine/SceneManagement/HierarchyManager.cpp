//
// Created by Simeon on 4/8/2024.
//

#include "Scene.h"
#include "HierarchyManager.h"
#include "Renderer.h"

std::shared_ptr<Camera> HierarchyManager::selectedCamera = nullptr;

void HierarchyManager::renderHierarchy(Renderer* renderer, Scene* scene) {
    ImGui::Begin("Hierarchy");

    if (renderer == nullptr) {
        std::cerr << "Renderer is null\n";
    } else {
        /*
        std::vector<std::shared_ptr<GameObject>> objects = renderer;

        for (const auto& object : objects) {
            if (std::dynamic_pointer_cast<SubGameObject>(object)) {
                // Retrieve child objects for SubGameObjects
                std::vector<std::shared_ptr<GameObject>> children = std::dynamic_pointer_cast<SubGameObject>(object)->getGameObjects();

                // Iterate over the child objects here.
                for (const auto& child : children) {
                    if (ImGui::Selectable(child->getName().c_str())) {
                        renderer->selectedObjects = object;
                    }
                }
            } else {
                // Not a SubGameObject, display directly
                if (ImGui::Selectable(object->getName().c_str())) {
                    renderer->selectedObjects = object;
                }
            }
        }
        */
        for (const auto& object : scene->objects) {
            if (ImGui::Selectable(object->getName().c_str())) {
                renderer->selectedObjects = object;
                selectedCamera = nullptr;
            }
        }

        std::shared_ptr<Camera> camera = renderer->getCamera();
        if (camera != nullptr) {
            if (ImGui::Selectable(camera->getName().c_str())) {
                selectedCamera = camera;
                renderer->selectedObjects = nullptr;
            }
        }
    }
    ImGui::End();
}
