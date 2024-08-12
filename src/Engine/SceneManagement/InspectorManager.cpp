//
// Created by Simeon on 4/8/2024.
//

#include "InspectorManager.h"

#include "imgui.h"
#include ".h/BoxCollider.h"
#include "Transform.h"
#include ".h/CapsuleCollider.h"
#include ".h/CylinderCollider.h"
#include ".h/SphereCollider.h"

void InspectorManager::renderInspector(const std::shared_ptr<GameObject>& selectedObject) {
    ImGui::Begin("Inspector");
    const auto transform = selectedObject->getComponent<Transform>();
    const auto boxCollider = selectedObject->getComponent<BoxCollider>();
    const auto cylinderCol = selectedObject->getComponent<CylinderCollider>();
    const auto capsuleCollider = selectedObject->getComponent<CapsuleCollider>();
    const auto sphereCol = selectedObject->getComponent<SphereCollider>();

    if (selectedObject) {
        ImGui::Text("Name: %s", selectedObject->getName().c_str());

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

        if (boxCollider) {
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

        if (cylinderCol) {
            ImGui::Text("CylinderCollider:");

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Radius");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Radius", &cylinderCol->radius, "%.2f");

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Height");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Height", &cylinderCol->height, "%.2f");
        }

        if (capsuleCollider) {
            ImGui::Text("CapsuleCollider:");

            /*
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Pos");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Pos", glm::value_ptr(capsuleCollider->m_position), "%.2f");

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Scale");
            ImGui::SameLine(100);
            ImGui::InputFloat("##Scale", &capsuleCollider->wireframeScale, 0.01f, 0.1f, "%.2f");*/
        }

        if (sphereCol) {
            ImGui::Text("SphereCollider:");


            ImGui::AlignTextToFramePadding();
            ImGui::Text("Pos");
            ImGui::SameLine(100);
            ImGui::InputFloat3("##Pos", glm::value_ptr(sphereCol->m_position), "%.2f");

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Scale");
            ImGui::SameLine(100);
            ImGui::InputFloat("##Scale", &sphereCol->m_radius, 0.01f, 0.1f, "%.2f");
        }
    }
    else {
        std::cerr << "selected object is null\n";
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
