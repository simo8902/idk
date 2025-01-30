//
// Created by Simeon on 4/8/2024.
//

#include "HierarchyManager.h"
#include "Renderer.h"
#include "Scene.h"
#include "LightManager.h"
#include "SelectionManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "DirectionalLight.h"
#include <imgui.h>
#include <iostream>

void HierarchyManager::setRenderer(Renderer* renderer) {
    this->renderer = renderer;
}

void HierarchyManager::setScene(const std::shared_ptr<Scene> & scene) {
    this->scene = scene;
}

void HierarchyManager::setLightManager(const std::shared_ptr<LightManager>& lightManager) {
    this->lightManager = lightManager;
}

void HierarchyManager::renderHierarchy() const {

    if (renderer == nullptr) {
        std::cerr << "Renderer is null\n";
    } else if (scene == nullptr) {
        std::cerr << "Scene is null\n";
    } else {
       // renderer->RenderContextMenu();

        for (size_t i = 0; i < scene->components.size(); ++i) {
            auto& object = scene->components[i];

            if (object->getName().empty()) {
                object->setName("Object " + std::to_string(i));
            }

            if (const bool &isSelected = (SelectionManager::getInstance().getSelectedComponent() == object);
                ImGui::Selectable((object->getName() + "##" + std::to_string(i)).c_str(), isSelected))
            {
                SelectionManager::getInstance().selectComponent(object);
            }
        }

        /*
        const std::shared_ptr<Camera> & camera = renderer->getCamera();
        if (camera != nullptr) {
            bool isSelected = SelectionManager::getInstance().selectedCamera == camera;

            if (ImGui::Selectable(camera->getName().c_str(), isSelected)) {
                //SelectionManager::getInstance().selectCamera(camera);
            }
        }*/

        /*
        if (lightManager) {
            const auto& lights = lightManager->getDirectionalLights();

            for (size_t i = 0; i < lights.size(); ++i) {
                std::shared_ptr<DirectionalLight> light = lights[i];

                if (light) {
                    bool isSelected = SelectionManager::getInstance().selectedLight == light;
                    if (ImGui::Selectable(light->getName().c_str(), isSelected)) {
                        SelectionManager::getInstance().selectLight(light);
                    }
                } else {
                    std::cerr << "DirectionalLight at index " << i << " is null\n";
                }
            }
        } else {
            std::cerr << "LightManager is null\n";
        }*/
    }

    if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) {
        if (!ImGui::IsAnyItemHovered()) {
            SelectionManager::getInstance().clearSelection();
        }
    }
}
