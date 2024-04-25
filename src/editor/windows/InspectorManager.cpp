//
// Created by Simeon on 4/8/2024.
//

#include "InspectorManager.h"
#include "../../components/colliders/BoxCollider.h"
#include "../../components/Transform.h"


void InspectorManager::renderInspector(const std::shared_ptr<GameObject>& selectedObject) {
     ImGui::Begin("Inspector");

    if (selectedObject) {
        ImGui::Text("Name: %s", selectedObject->getName().c_str());

        auto transform = selectedObject->getComponent<Transform>();
        if (transform) {
            ImGui::Text("Transform:");

            //Position
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Position");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##TransformPos", glm::value_ptr(transform->position), "%.2f");

            //Rotation
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Rotation");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Rotation", glm::value_ptr(transform->rotation), "%.2f");

            //Scale
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Scale");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Scale", glm::value_ptr(transform->m_scale), "%.2f");
        }

        auto boxCollider = selectedObject->getComponent<BoxCollider>();
        if (boxCollider) {
            // Display editable BoxCollider properties
            ImGui::Text("BoxCollider:");

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Min");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Min", glm::value_ptr(boxCollider->m_worldMin), "%.2f");

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Max");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Max", glm::value_ptr(boxCollider->m_worldMax), "%.2f");
        }
    }
    ImGui::End();
 }


void InspectorManager::renderInspector(const std::shared_ptr<Camera>& camera) {
    ImGui::Begin("Inspector");

    if (camera){
        ImGui::Text("Name: %s", camera->getName().c_str());

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Position");
        ImGui::SameLine(100);
        ImGui::InputFloat3("##CameraPos", glm::value_ptr(camera->m_position), "%.2f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Forward");
        ImGui::SameLine(100);
        ImGui::InputFloat3("##CameraForward", glm::value_ptr(camera->m_forwardVec), "%.2f");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Up");
        ImGui::SameLine(100);
        ImGui::InputFloat3("##CameraUp", glm::value_ptr(camera->m_upVec), "%.2f");
    }
    ImGui::End();
}
