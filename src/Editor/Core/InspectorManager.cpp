//
// Created by Simeon on 4/8/2024.
//

#include "InspectorManager.h"

#include <unordered_set>

#include "AssetManager.h"
#include "DirectionalLight.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "DragAndDropPayload.h"
#include "SelectionManager.h"
#include "imgui.h"

void InspectorManager::renderInspector() {
    auto& selectionManager = SelectionManager::getInstance();

    if (selectionManager.selectedComponent) {
        renderComponentObjectsInspector(selectionManager.selectedComponent);
    } else if (selectionManager.selectedLight) {
        renderLightInspector(selectionManager.selectedLight);
    } else if (selectionManager.selectedCamera) {
        renderCameraInspector(selectionManager.selectedCamera);
    } else if (selectionManager.selectedMesh) {
        renderMeshInspector(selectionManager.selectedMesh);
    } else if (selectionManager.selectedMaterial) {
        renderMaterialInspector(selectionManager.selectedMaterial);
    } else {
        ImGui::Text("No object selected.");
    }
}

void InspectorManager::renderComponentObjectsInspector(const std::shared_ptr<Component> &componentObj)
{
    if (!componentObj) return;

    static char nameBuffer[256];
    strcpy(nameBuffer, componentObj->getName().c_str());
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
        componentObj->setName(std::string(nameBuffer));
    }

    auto transform = componentObj->getComponent<Transform>();
    if (transform) {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (glm::vec3 position = transform->getPosition(); ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
                transform->setPosition(position);
            }

            if (glm::vec3 rotation = glm::degrees(glm::eulerAngles(transform->getRotation())); ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f)) {
                glm::vec3 radians = glm::radians(rotation);
                transform->setRotation(glm::quat(radians));
            }

            if (glm::vec3 scale = transform->getScale(); ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f)) {
                transform->setScale(scale);
            }
        }
    }

    const auto& components = componentObj->getComponents();
    std::unordered_set<std::string> displayedComponents;

    for (const auto& component : components) {
        if (std::dynamic_pointer_cast<Transform>(component)) {
            continue;
        }

        if (auto meshFilter = std::dynamic_pointer_cast<MeshFilter>(component)) {
            if (!displayedComponents.contains("MeshFilter")) {
                if (ImGui::CollapsingHeader("Mesh Filter", ImGuiTreeNodeFlags_DefaultOpen)) {
                    if (meshFilter->mesh) {
                        ImGui::Text("Mesh: %s", meshFilter->getName().c_str());
                    } else {
                        ImGui::Text("No mesh assigned.");
                    }
                }
                displayedComponents.insert("MeshFilter");
            }
        }
        else if (auto meshRenderer = std::dynamic_pointer_cast<MeshRenderer>(component)) {
            if (!displayedComponents.contains("MeshRenderer")) {
                if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                    /*
                    if (meshRenderer->getMaterial()) {
                        ImGui::Text("Material: %s", meshRenderer->getMaterial()->getName().c_str());
                    } else {
                        ImGui::Text("No material assigned.");
                    }*/

                    ImGui::Text("Drag and drop a material here to assign.");
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL_PAYLOAD")) {
                            const char* materialUUIDCStr = static_cast<const char*>(payload->Data);
                            std::string materialUUID(materialUUIDCStr);

                            // std::shared_ptr<Material> newMaterial = AssetManager::getInstance().getMaterialByUUID(materialUUID);

                            /*
                            if (newMaterial) {
                                meshRenderer->setMaterial(newMaterial);
                              //  std::cout << "Assigned material: " << newMaterial->getName() << " to MeshRenderer." << std::endl;
                            } else {
                                std::cerr << "Material not found: " << materialUUID << std::endl;
                            }*/
                        }
                        ImGui::EndDragDropTarget();

                    }
                }
                displayedComponents.insert("MeshRenderer");
            }
        } else if (auto collider = std::dynamic_pointer_cast<Collider>(component)) {
            if (displayedComponents.find("Collider") == displayedComponents.end()) {
                if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Text("Collider Type: %s", typeid(*collider).name());
                }
                displayedComponents.insert("Collider");
            }
        } else {
            std::string componentName = typeid(*component).name();
            if (displayedComponents.find(componentName) == displayedComponents.end()) {
                if (ImGui::CollapsingHeader(componentName.c_str())) {
                    ImGui::Text("Component details go here.");
                }
                displayedComponents.insert(componentName);
            }
        }
    }
}

void InspectorManager::renderLightInspector(const std::shared_ptr<Light>& light) {
    /*
    if (!light) return;

    // Allow renaming the light
    static char nameBuffer[256];
    strcpy(nameBuffer, light->getName().c_str());
    
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
        light->setName(std::string(nameBuffer));
    }

    // Display and edit the Transform component if present
    auto transform = light->getComponent<Transform>();
    if (transform) {
        renderTransformComponent(transform);
    }

    // Display light-specific properties
    if (auto dirLight = std::dynamic_pointer_cast<DirectionalLight>(light)) {
        if (ImGui::CollapsingHeader("Directional Light Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
            glm::vec3 direction = dirLight->getDirection();
            if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.1f)) {
                dirLight->setDirection(glm::normalize(direction));
            }

            glm::vec3 ambient = dirLight->getAmbient();
            if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient))) {
                dirLight->setAmbient(ambient);
            }

            glm::vec3 diffuse = dirLight->getDiffuse();
            if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse))) {
                dirLight->setDiffuse(diffuse);
            }

            glm::vec3 specular = dirLight->getSpecular();
            if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular))) {
                dirLight->setSpecular(specular);
            }
        }
    }

    else if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light)) {
        if (ImGui::CollapsingHeader("Point Light Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
            glm::vec3 position = pointLight->getPosition();
            if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
                pointLight->setPosition(position);
            }

            glm::vec3 ambient = pointLight->getAmbient();
            if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient))) {
                pointLight->setAmbient(ambient);
            }

            glm::vec3 diffuse = pointLight->getDiffuse();
            if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse))) {
                pointLight->setDiffuse(diffuse);
            }

            glm::vec3 specular = pointLight->getSpecular();
            if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular))) {
                pointLight->setSpecular(specular);
            }

            float constant = pointLight->getConstant();
            if (ImGui::DragFloat("Constant", &constant, 0.01f, 0.0f, 1.0f)) {
                pointLight->setConstant(constant);
            }

            float linear = pointLight->getLinear();
            if (ImGui::DragFloat("Linear", &linear, 0.001f, 0.0f, 1.0f)) {
                pointLight->setLinear(linear);
            }

            float quadratic = pointLight->getQuadratic();
            if (ImGui::DragFloat("Quadratic", &quadratic, 0.0001f, 0.0f, 1.0f)) {
                pointLight->setQuadratic(quadratic);
            }
        }
    } else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(light)) {
        if (ImGui::CollapsingHeader("Spot Light Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
            glm::vec3 position = spotLight->getPosition();
            if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
                spotLight->setPosition(position);
            }

            glm::vec3 direction = spotLight->getDirection();
            if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.1f)) {
                spotLight->setDirection(glm::normalize(direction));
            }

            glm::vec3 ambient = spotLight->getAmbient();
            if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient))) {
                spotLight->setAmbient(ambient);
            }

            glm::vec3 diffuse = spotLight->getDiffuse();
            if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse))) {
                spotLight->setDiffuse(diffuse);
            }

            glm::vec3 specular = spotLight->getSpecular();
            if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular))) {
                spotLight->setSpecular(specular);
            }

            float cutoff = glm::degrees(spotLight->getCutOff());
            if (ImGui::DragFloat("Cut Off", &cutoff, 0.1f, 0.0f, 90.0f)) {
                spotLight->setCutOff(glm::radians(cutoff));
            }

            float outerCutoff = glm::degrees(spotLight->getOuterCutOff());
            if (ImGui::DragFloat("Outer Cut Off", &outerCutoff, 0.1f, 0.0f, 90.0f)) {
                spotLight->setOuterCutOff(glm::radians(outerCutoff));
            }

            float constant = spotLight->getConstant();
            if (ImGui::DragFloat("Constant", &constant, 0.01f, 0.0f, 1.0f)) {
                spotLight->setConstant(constant);
            }

            float linear = spotLight->getLinear();
            if (ImGui::DragFloat("Linear", &linear, 0.001f, 0.0f, 1.0f)) {
                spotLight->setLinear(linear);
            }

            float quadratic = spotLight->getQuadratic();
            if (ImGui::DragFloat("Quadratic", &quadratic, 0.0001f, 0.0f, 1.0f)) {
                spotLight->setQuadratic(quadratic);
            }
        }
     else {
        ImGui::Text("Unknown light type.");
    }*/
}

void InspectorManager::renderCameraInspector(const std::shared_ptr<Camera>& camera) {
    if (!camera) return;

    // Allow renaming the camera
    static char nameBuffer[256];
    strcpy(nameBuffer, camera->getName().c_str());
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
        camera->setName(std::string(nameBuffer));
    }

    // Display and edit camera properties
    if (ImGui::CollapsingHeader("Camera Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
        float fov = camera->getFOV();
        if (ImGui::DragFloat("Field of View", &fov, 0.1f, 1.0f, 179.0f)) {
            camera->setFOV(fov);
        }

        float nearPlane = camera->getNearPlane();
        float farPlane = camera->getFarPlane();
        if (ImGui::DragFloatRange2("Clipping Planes", &nearPlane, &farPlane, 0.1f, 0.01f, 1000.0f)) {
            camera->setNearPlane(nearPlane);
            camera->setFarPlane(farPlane);
        }

        glm::vec3 position = camera->getPosition();
        if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
            camera->setPosition(position);
        }

        float yaw = camera->getYaw();
        float pitch = camera->getPitch();

        if (ImGui::DragFloat("Yaw", &yaw, 0.1f, -360.0f, 360.0f)) {
            camera->setYaw(yaw);
        }
        if (ImGui::DragFloat("Pitch", &pitch, 0.1f, -89.0f, 89.0f)) {
            camera->setPitch(pitch);
        }
    }
}

void InspectorManager::renderMeshInspector(const std::shared_ptr<Mesh>& mesh) {
    if (!mesh) return;

    ImGui::Text("Mesh: %s", mesh->getName().c_str());

    ImGui::Text("Vertex Count: %zu", mesh->getVertices().size());
    ImGui::Text("Index Count: %zu", mesh->getIndices().size());
}

void InspectorManager::renderMaterialInspector(const std::shared_ptr<Material>& material) {
    if (!material) {
        ImGui::Text("No material selected.");
        return;
    }

    ImGui::Begin("Inspector");

    ImGui::Text("Material: %s", material->getName().c_str());
    ImGui::Separator();

    std::shared_ptr<Shader> shader = material->getShader();
    if (shader) {
       // ImGui::Text("Shader: %s", shader->getName().c_str());
      //  ImGui::Text("UUID: %s", shader->getUUIDStr().c_str());
    } else {
        ImGui::Text("No shader assigned.");
    }

    ImGui::Separator();

    ImGui::Text("Drag and drop a shader here to assign.");
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_SHADER)) {
            const char* shaderUUIDCStr = static_cast<const char*>(payload->Data);
            std::string shaderUUID(shaderUUIDCStr);

            /*
            std::shared_ptr<Shader> newShader = AssetManager::getInstance().getShaderByUUID(shaderUUID);
            if (newShader) {
                material->assignShader(newShader);
              //  std::cout << "[InspectorManager] Assigned shader: " << newShader->getName()
              //            << " to material: " << material->getName() << std::endl;
            } else {
                std::cerr << "[InspectorManager] Shader not found: " << shaderUUID << std::endl;
                ImGui::OpenPopup("Shader Not Found");
            }*/
        }
        ImGui::EndDragDropTarget();
    }

ImGui::Separator();

if (shader) {
    ImGui::Text("Shader Parameters:");
    ImGui::BeginChild("ShaderParameters", ImVec2(0, 300), true);

    ImGui::EndChild();
} else {
    ImGui::Text("No shader assigned.");
}

ImGui::End();
}


void InspectorManager::renderTransformComponent(const std::shared_ptr<Transform>& transform) {
    if (!transform) return;

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        glm::vec3 position = transform->getPosition();
        if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
            transform->setPosition(position);
        }

        glm::vec3 rotation = glm::degrees(glm::eulerAngles(transform->getRotation()));
        if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f)) {
            glm::vec3 radians = glm::radians(rotation);
            transform->setRotation(glm::quat(radians));
        }

        glm::vec3 scale = transform->getScale();
        if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f)) {
            transform->setScale(scale);
        }
    }
}