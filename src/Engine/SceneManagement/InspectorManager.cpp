//
// Created by Simeon on 4/8/2024.
//

#include "InspectorManager.h"

#include "HierarchyManager.h"
#include "imgui.h"
#include "Light.h"
#include "LightManager.h"
#include ".h/BoxCollider.h"
#include "Transform.h"
#include ".h/CapsuleCollider.h"
#include ".h/CylinderCollider.h"
#include ".h/SphereCollider.h"
#include "DirectionalLight.h"

class DirectionalLight;

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

    if (camera) {
        ImGui::Text("Name: %s", camera->getName().c_str());

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Position");
        ImGui::SameLine(100);

        // Create a temporary copy of the position to edit
        glm::vec3 tempPosition = camera->m_position;
        if (ImGui::InputFloat3("##CameraPos", glm::value_ptr(tempPosition), "%.2f")) {
            // Only update the camera if the position has changed
            camera->m_position = tempPosition;
            camera->updateViewMatrix(); // Update the view matrix after changing position
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Forward");
        ImGui::SameLine(100);

        // Similarly, update the forward vector
        glm::vec3 tempForward = camera->m_forwardVec;
        if (ImGui::InputFloat3("##CameraForward", glm::value_ptr(tempForward), "%.2f")) {
            camera->m_forwardVec = glm::normalize(tempForward);
            camera->updateViewMatrix(); // Update view matrix after changing forward vector
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Up");
        ImGui::SameLine(100);

        // Similarly, update the up vector
        glm::vec3 tempUp = camera->m_upVec;
        if (ImGui::InputFloat3("##CameraUp", glm::value_ptr(tempUp), "%.2f")) {
            camera->m_upVec = glm::normalize(tempUp);
            camera->updateViewMatrix(); // Update view matrix after changing up vector
        }
    }
    ImGui::End();
}

void InspectorManager::renderInspector(const std::shared_ptr<Light>& light) {
    ImGui::Begin("Inspector");


    if (light) {
    auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light);
    if (directionalLight) {
        ImGui::Text("Selected Light: %s", directionalLight->name.c_str());

        if (directionalLight->hasTransform()) {
            glm::vec3 position = directionalLight->transform->getPosition();
            glm::vec3 rotation = glm::degrees(glm::eulerAngles(directionalLight->transform->getRotation()));

            if (ImGui::InputFloat3("Position", glm::value_ptr(position))) {
                directionalLight->transform->setPosition(position);
            }

            if (ImGui::InputFloat3("Rotation", glm::value_ptr(rotation))) {
                glm::vec3 rotationRadians = glm::radians(rotation);
                directionalLight->transform->setRotation(rotationRadians);
            }

            glm::vec3 direction = directionalLight->direction;
            ImGui::Text("Direction: %.2f, %.2f, %.2f", direction.x, direction.y, direction.z);
        } else {
            ImGui::Text("Transform component is missing.");
        }

        // Light properties (ambient, diffuse, specular)
        glm::vec3 ambient = directionalLight->ambient;
        glm::vec3 diffuse = directionalLight->diffuse;
        glm::vec3 specular = directionalLight->specular;

        // Input fields for light properties
        if (ImGui::InputFloat3("Ambient", glm::value_ptr(ambient))) {
            directionalLight->setAmbient(ambient);
        }

        if (ImGui::InputFloat3("Diffuse", glm::value_ptr(diffuse))) {
            directionalLight->setDiffuse(diffuse);
        }

        if (ImGui::InputFloat3("Specular", glm::value_ptr(specular))) {
            directionalLight->setSpecular(specular);
        }
    } else {
        ImGui::Text("Selected light is not a directional light.");
    }
} else {
    ImGui::Text("No light selected.");
}




    ImGui::End();
}