//
// Created by Simeon on 4/8/2024.
//

#include "Scene.h"
#include "HierarchyManager.h"
#include "Renderer.h"

std::shared_ptr<Camera> HierarchyManager::selectedCamera = nullptr;
std::shared_ptr<Light> HierarchyManager::selectedLight = nullptr;

void HierarchyManager::renderHierarchy(Renderer* renderer, Scene* scene,std::shared_ptr<LightManager> lightManager) {
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

        renderer->RenderContextMenu();

        for (const auto& object : scene->objects) {
            if (ImGui::Selectable(object->getName().c_str())) {
                renderer->selectedObjects = object;
                selectedCamera = nullptr;
                renderer->selectedLight = nullptr;
            }
        }

        std::shared_ptr<Camera> camera = renderer->getCamera();
        if (camera != nullptr) {
            if (ImGui::Selectable(camera->getName().c_str())) {
                selectedCamera = camera;
                renderer->selectedObjects = nullptr;
                renderer->selectedLight = nullptr;
            }
        }



        std::shared_ptr<LightManager> lightManager = renderer->getLight();
        if (lightManager) {
            const auto& lights = lightManager->getDirectionalLights(); // Use the appropriate method to get directional lights

            for (size_t i = 0; i < lights.size(); ++i) {
                std::shared_ptr<DirectionalLight> light = lights[i];

                if (light) {
                    if (ImGui::Selectable(light->getName().c_str())) {
                        renderer->selectedLight = light; // Select the light in the renderer
                        selectedCamera = nullptr; // Reset selected camera
                        renderer->selectedObjects = nullptr; // Reset selected objects
                    }
                } else {
                    std::cerr << "DirectionalLight at index " << i << " is null\n";
                }
            }
        } else {
            std::cerr << "LightManager is null\n";
        }


/*
        for (size_t i = 0; i < lightManager->lights.size(); ++i) {
            auto& light = lightManager->lights[i];

            ImGui::PushID(i);

            char buffer[128];
            strncpy(buffer, light->name.c_str(), sizeof(buffer));
            if (ImGui::InputText("Light Name", buffer, sizeof(buffer))) {
                light->name = std::string(buffer);
            }

            ImGui::PopID();
        }*/

    }
    ImGui::End();
}
