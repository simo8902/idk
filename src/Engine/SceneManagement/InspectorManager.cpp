//
// Created by Simeon on 4/8/2024.
//

#include "InspectorManager.h"

#include <unordered_set>

#include "AssetManager.h"
#include "DirectionalLight.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include ".h/Collider.h"
#include "DragAndDropPayload.h"

void InspectorManager::renderInspector() {
    ImGui::Begin("Inspector");

    auto& selectionManager = SelectionManager::getInstance();

    if (selectionManager.selectedGameObject) {
        renderGameObjectInspector(selectionManager.selectedGameObject);
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

    ImGui::End();
}
void InspectorManager::renderGameObjectInspector(const std::shared_ptr<GameObject>& gameObject) {
   if (!gameObject) return;

    static char nameBuffer[256];
    strcpy(nameBuffer, gameObject->getName().c_str());
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
        gameObject->setName(std::string(nameBuffer));
    }

    auto transform = gameObject->getComponent<Transform>();
    if (transform) {
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

    const auto& components = gameObject->getComponents();
    std::unordered_set<std::string> displayedComponents;

    for (const auto& component : components) {
        if (std::dynamic_pointer_cast<Transform>(component)) {
            continue;
        } else if (auto meshFilter = std::dynamic_pointer_cast<MeshFilter>(component)) {
            if (displayedComponents.find("MeshFilter") == displayedComponents.end()) {
                if (ImGui::CollapsingHeader("Mesh Filter", ImGuiTreeNodeFlags_DefaultOpen)) {
                    if (meshFilter->mesh) {
                        ImGui::Text("Mesh: %s", meshFilter->mesh->getName().c_str());
                    } else {
                        ImGui::Text("No mesh assigned.");
                    }
                }
                displayedComponents.insert("MeshFilter");
            }
        }
        else if (auto meshRenderer = std::dynamic_pointer_cast<MeshRenderer>(component)) {
            if (displayedComponents.find("MeshRenderer") == displayedComponents.end()) {
                if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                    if (meshRenderer->getMaterial()) {
                        ImGui::Text("Material: %s", meshRenderer->getMaterial()->getName().c_str());
                    } else {
                        ImGui::Text("No material assigned.");
                    }

                    // Drag and Drop Target (for receiving materials)
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_MATERIAL)) {
                            IM_ASSERT(payload->DataSize == sizeof(std::shared_ptr<Material>));
                            auto material = *static_cast<std::shared_ptr<Material>*>(payload->Data);
                            meshRenderer->setMaterial(material);
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
    if (!light) return;

    // Allow renaming the light
    static char nameBuffer[256];
    strcpy(nameBuffer, light->getName().c_str());
    /*
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
        light->setName(std::string(nameBuffer));
    }*/

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

    /*else if (auto pointLight = std::dynamic_pointer_cast<PointLight>(light)) {
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
        }*/
     else {
        ImGui::Text("Unknown light type.");
    }
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

        // Display and edit position
        glm::vec3 position = camera->getPosition();
        if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
            camera->setPosition(position);
        }

        // Display and edit rotation (Yaw and Pitch)
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

    // Display mesh properties
    ImGui::Text("Vertex Count: %zu", mesh->getVertices().size());
    ImGui::Text("Index Count: %zu", mesh->getIndices().size());

    // Optionally, display other mesh information
}

void InspectorManager::renderMaterialInspector(const std::shared_ptr<Material>& material) {
      if (!material) {
        std::cout << "InspectorManager: No material selected." << std::endl;
        return;
    }

    std::cout << "InspectorManager: Rendering inspector for material: " << material->getName() << std::endl;

    ImGui::Begin("Inspector");

    // Display Material Name
    ImGui::Text("Material: %s", material->getName().c_str());
    ImGui::Separator();

    // Shader Assignment Section
    ImGui::Text("Assigned Shader:");
    if (auto shader = material->getShader()) {
        std::string shaderName;
        const auto& shaders = AssetManager::getInstance().getShaders();
        for (const auto& [name, shaderPtr] : shaders) {
            if (shaderPtr->GetProgramID() == shader->GetProgramID()) {
                shaderName = name;
                break;
            }
        }
        if (!shaderName.empty()) {
            ImGui::Text("Shader: %s", shaderName.c_str());
        }
        else {
            ImGui::Text("Shader Program ID: %u", shader->GetProgramID());
        }
    }
    else {
        ImGui::Text("No shader assigned.");
    }

    // Drop Target Area for Shader Assignment
    ImGui::Separator();
    ImGui::Text("Drag and drop a shader here to assign.");

    // Define a drop area (e.g., a button)
    if (ImGui::Button("Drop Shader Here", ImVec2(-1, 50))) {
        // Optional: Handle button click if needed
    }

    // Handle Drop
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_SHADER)) {
            if (payload->Data && payload->DataSize > 0) {
                const char* shaderNameCStr = static_cast<const char*>(payload->Data);
                std::string shaderNameStr(shaderNameCStr, payload->DataSize - 1); // Exclude null terminator

                std::cout << "InspectorManager: Received shader name: " << shaderNameStr << std::endl;

                // Retrieve the Shader instance from AssetManager
                try {
                    auto shader = AssetManager::getInstance().getShader(shaderNameStr);
                    if (shader) {
                        // Assign shader to material
                        material->assignShader(shader);
                        std::cout << "InspectorManager: Assigned shader " << shaderNameStr << " to material " << material->getName() << std::endl;
                    }
                    else {
                        std::cerr << "InspectorManager Error: Shader " << shaderNameStr << " not found in AssetManager." << std::endl;
                        // Optionally, display a popup error message
                        ImGui::OpenPopup("Shader Not Found");
                    }
                }
                catch (const std::runtime_error& e) {
                    std::cerr << "InspectorManager Error: " << e.what() << std::endl;
                    // Optionally, display a popup error message
                    ImGui::OpenPopup("Shader Error");
                }
            }
            else {
                std::cerr << "InspectorManager Error: Received invalid shader payload." << std::endl;
                // Optionally, display a popup error message
                ImGui::OpenPopup("Invalid Payload");
            }
        }

        ImGui::EndDragDropTarget();
    }

    // Display Assigned Shader Information
    ImGui::Separator();
    if (auto shader = material->getShader()) {
        std::string shaderName;
        const auto& shaders = AssetManager::getInstance().getShaders();
        for (const auto& [name, shaderPtr] : shaders) {
            if (shaderPtr->GetProgramID() == shader->GetProgramID()) {
                shaderName = name;
                break;
            }
        }
        if (!shaderName.empty()) {
            ImGui::Text("Assigned Shader: %s (Program ID: %u)", shaderName.c_str(), shader->GetProgramID());
        }
        else {
            ImGui::Text("Assigned Shader Program ID: %u", shader->GetProgramID());
        }
    }
    else {
        ImGui::Text("No shader assigned to this material.");
        ImGui::Separator();
        // Provide instructions or options to assign a shader
        ImGui::Text("Drag and drop a shader from the Project Explorer to assign.");
    }

    // Handle Popups for Errors
    if (ImGui::BeginPopup("Shader Not Found")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Shader not found.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Shader Error")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error assigning shader to material.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Invalid Payload")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid shader payload received.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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