//
// Created by Simeon on 4/8/2024.
//

#include "ProjectExplorer.h"

#include <fstream>

#include "AssetManager.h"
#include "Material.h"
#include "SelectionManager.h"
#include "Shader.h"
#include <filesystem>
#include "DragAndDropPayload.h"
#include "imgui.h"

ProjectExplorer::ProjectExplorer() = default;

void ProjectExplorer::renderProjectExplorer() {
    ImGui::Begin("Project Explorer");

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup("Context Menu");
    }

    if (ImGui::BeginPopup("Context Menu")) {

        if(ImGui::MenuItem("Material")) {
            std::string materialName = "Material_" + std::to_string(AssetManager::getInstance().getMaterials().size() + 1);
            auto newMaterial = std::make_shared<Material>();
            newMaterial->setName(materialName);

            AssetManager::getInstance().addMaterial(materialName, newMaterial);
            std::cout << "Created new material: " << materialName << std::endl;
        }

        if (ImGui::MenuItem("New Standard Surface Shader")) {
            std::string shaderBaseName = "StandardSurfaceShader";
            int shaderCount = AssetManager::getInstance().getShaders().size() + 1;
            std::string shaderName = shaderBaseName + "_" + std::to_string(shaderCount);

            try {
                EnsureAssetsFolderExists();

                std::string shaderDir = std::string(SOURCE_DIR) + "/assets/shaders/";
                std::string shaderPath = shaderDir + shaderName + ".glsl";

                if (std::filesystem::exists(shaderPath)) {
                    std::cerr << "Shader file already exists: " << shaderPath << std::endl;
                    ImGui::OpenPopup("Shader Exists");
                }
                else {
                    CreateStandardShaderFile(shaderPath);

                    auto newShader = std::make_shared<Shader>(shaderPath);

                    AssetManager::getInstance().addShader(shaderName, newShader);

                    std::cout << "Shader " << shaderName << " loaded and compiled successfully." << std::endl;
                }
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error creating or compiling shader: " << e.what() << std::endl;
                ImGui::OpenPopup("Shader Error");
            }
        }



        ImGui::EndPopup();
    }

    const auto& shaders = AssetManager::getInstance().getShaders();
    if (!shaders.empty()) {
        ImGui::Separator();
        ImGui::Text("Shaders:");
        for (const auto& [shaderName, shaderProgram] : shaders) {
            ImGui::Selectable(shaderName.c_str());

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload(PAYLOAD_SHADER, shaderName.c_str(), shaderName.size() + 1, ImGuiCond_Once);
                ImGui::Text("Dragging %s", shaderName.c_str());
                std::cout << "Initiated drag for shader: " << shaderName << std::endl;
                ImGui::EndDragDropSource();
            }
        }
    }

    const auto& materials = AssetManager::getInstance().getMaterials();

    bool clickedInsideSelectable = false;

        for (const auto& entry : materials) {
            const auto& material = entry.second;
            bool isSelected = (SelectionManager::getInstance().selectedMaterial == material);
            if (ImGui::Selectable(material->getName().c_str(), isSelected)) {
                SelectionManager::getInstance().selectMaterial(material);
                clickedInsideSelectable = true;
            }

            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("MATERIAL_COMPONENT", &material, sizeof(std::shared_ptr<Material>));
                ImGui::Text("Dragging %s", material->getName().c_str());
                ImGui::EndDragDropSource();
            }
        }


    if (ImGui::IsMouseClicked(0) && !clickedInsideSelectable && ImGui::IsWindowHovered()) {
        SelectionManager::getInstance().clearSelection();
    }

    ImGui::End();
}

void ProjectExplorer::CreateStandardShaderFile(const std::string& shaderPath) {
    std::ofstream shaderFile(shaderPath);
    if (!shaderFile.is_open()) {
        throw std::runtime_error("Failed to create shader file: " + shaderPath);
    }

    // Vertex Shader Section
    shaderFile << "#shader vertex\n"
               << "#version 330 core\n"
               << "layout(location = 0) in vec3 aPos;\n"
               << "layout(location = 1) in vec3 aColor;\n\n"
               << "out vec3 ourColor;\n\n"
               << "uniform mat4 model;\n"
               << "uniform mat4 view;\n"
               << "uniform mat4 projection;\n\n"
               << "void main()\n"
               << "{\n"
               << "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
               << "    ourColor = aColor;\n"
               << "}\n\n";

    // Fragment Shader Section
    shaderFile << "#shader fragment\n"
               << "#version 330 core\n"
               << "in vec3 ourColor;\n"
               << "out vec4 FragColor;\n\n"
               << "void main()\n"
               << "{\n"
               << "    FragColor = vec4(ourColor, 1.0);\n"
               << "}\n";

    shaderFile.close();

    std::cout << "Created combined shader file: " << shaderPath << std::endl;
}

void ProjectExplorer::EnsureAssetsFolderExists() {
    std::string projectDir = std::string(SOURCE_DIR);
    std::string shadersPath = projectDir + "/assets/shaders/";

    try {
        if (!std::filesystem::exists(shadersPath)) {
            std::cout << "Creating shaders folder: " << shadersPath << std::endl;
            std::filesystem::create_directories(shadersPath);
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Failed to create shaders directory: " + std::string(e.what()));
    }
}