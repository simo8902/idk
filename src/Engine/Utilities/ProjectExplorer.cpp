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
#include "imgui.h"

ProjectExplorer::ProjectExplorer()
    : shadersLoaded(false), clickedInsideSelectable(false) {
}

void ProjectExplorer::renderProjectExplorer() {
    clickedInsideSelectable = false;

    EnsureAssetsFolderExists();

    if (!shadersLoaded) {
        loadShadersFromDirectory();
        shadersLoaded = true;
    }

    handleContextMenu();

    displayShaders();
    displayMaterials();

    if (ImGui::BeginPopup("Shader Error")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error assigning shader to material.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            std::cout << "[ProjectExplorer] Closed 'Shader Error' popup." << std::endl;
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Shader Not Found")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Shader not found.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            std::cout << "[ProjectExplorer] Closed 'Shader Not Found' popup." << std::endl;
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Invalid Payload")) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid shader payload received.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            std::cout << "[ProjectExplorer] Closed 'Invalid Payload' popup." << std::endl;
        }
        ImGui::EndPopup();
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !clickedInsideSelectable && ImGui::IsWindowHovered()) {
        SelectionManager::getInstance().clearSelection();
    }
}

void ProjectExplorer::CreateAndAddShader(const std::string& shaderPath, const std::string& shaderName) {
    try {
        auto newShader = std::make_shared<Shader>(shaderPath);
        newShader->setName(shaderName);

        AssetManager::getInstance().addShader(newShader);

        userShaderUUIDs.insert(newShader->getUUID());

        std::cout << "[ProjectExplorer] Shader " << shaderName << " created and loaded successfully with UUID: " << newShader->getUUID() << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "[ProjectExplorer] Error creating or compiling shader: " << e.what() << std::endl;
        ImGui::OpenPopup("Shader Error");
    }
}

void ProjectExplorer::loadShadersFromDirectory() {
    std::string shaderDir = std::string(SOURCE_DIR) + "/assets/shaders/";
    std::cout << "[ProjectExplorer] Loading shaders from directory: " << shaderDir << std::endl;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(shaderDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".glsl") {
                std::string shaderFileName = entry.path().filename().string();
                std::string shaderName = entry.path().stem().string();

                bool shaderExists = false;
                for (const auto& [uuidStr, shaderPtr] : AssetManager::getInstance().getShaders()) {
                    if (shaderPtr->getName() == shaderName) {
                        shaderExists = true;
                        break;
                    }
                }

                if (!shaderExists) {
                    std::string shaderFilePath = shaderDir + shaderFileName;

                    try {
                        std::shared_ptr<Shader> newShader = std::make_shared<Shader>(shaderFilePath);
                        newShader->setName(shaderName);

                        AssetManager::getInstance().addShader(newShader);

                        userShaderUUIDs.insert(newShader->getUUID());

                        std::cout << "[ProjectExplorer] Shader " << shaderName << " loaded and added to AssetManager." << std::endl;
                    } catch (const std::runtime_error& e) {
                        std::cerr << "[ProjectExplorer] Error loading shader from file: " << e.what() << std::endl;
                        ImGui::OpenPopup("Shader Error");
                    }
                } else {
                    if (existingShadersReported.find(shaderName) == existingShadersReported.end()) {
                        std::cout << "[ProjectExplorer] Shader " << shaderName << " already exists in AssetManager." << std::endl;
                        existingShadersReported.insert(shaderName);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ProjectExplorer] Error scanning shader directory: " << e.what() << std::endl;
    }
}


void ProjectExplorer::displayMaterials() {
    const auto& materials = AssetManager::getInstance().getMaterials();

    if (!materials.empty()) {
        ImGui::Separator();
        ImGui::Text("Materials (Loaded in AssetManager):");
        for (const auto& [uuidStr, material] : materials) {
            std::string displayName = material->getName() + " [" + uuidStr + "]";

            bool isSelected = (SelectionManager::getInstance().getSelectedMaterial() == material);
            if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                SelectionManager::getInstance().selectMaterial(material);
                clickedInsideSelectable = true;
                std::cout << "[ProjectExplorer] Selected material: " << material->getName()  << " with UUID: " << uuidStr << std::endl;
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("MATERIAL_PAYLOAD", uuidStr.c_str(), uuidStr.size() + 1, ImGuiCond_Once);
                ImGui::Text("Dragging %s", material->getName().c_str());

               // std::cout << "[ProjectExplorer] Initiated drag for material: " << material->getName() << " with UUID: " << uuidStr << std::endl;
                ImGui::EndDragDropSource();
            }
        }
    } else {
        std::cout << "[ProjectExplorer] No materials loaded in AssetManager." << std::endl;
    }
}

void ProjectExplorer::handleContextMenu() {
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup("Context Menu");
        std::cout << "[ProjectExplorer] Opened 'Context Menu' popup." << std::endl;
    }

    if (ImGui::BeginPopup("Context Menu")) {
        if (ImGui::MenuItem("Material")) {
            std::string materialName = "Material_" + std::to_string(AssetManager::getInstance().getMaterials().size() + 1);
            std::shared_ptr<Material> newMaterial = std::make_shared<Material>(materialName);

            AssetManager::getInstance().addMaterial(newMaterial);
            std::cout << "[ProjectExplorer] Created new material: " << materialName << std::endl;

            clickedInsideSelectable = true;
        }

        if (ImGui::MenuItem("New Standard Surface Shader")) {
            std::string shaderBaseName = "StandardSurfaceShader";
            std::string shaderDir = std::string(SOURCE_DIR) + "/assets/shaders/";

            int shaderCount = 1;
            std::string shaderPath;

            while (true) {
                std::string potentialFileName = shaderBaseName + "_" + std::to_string(shaderCount) + ".glsl";
                shaderPath = shaderDir + potentialFileName;

                if (!std::filesystem::exists(shaderPath)) {
                    break;
                }
                shaderCount++;
            }

            try {
                EnsureAssetsFolderExists();

                std::cout << "[ProjectExplorer] Creating shader file at: " << shaderPath << std::endl;
                CreateStandardShaderFile(shaderPath);

                std::string shaderName = shaderBaseName + "_" + std::to_string(shaderCount);

                bool shaderExists = false;
                for (const auto& [uuidStr, shaderPtr] : AssetManager::getInstance().getShaders()) {
                    if (shaderPtr->getName() == shaderName) {
                        shaderExists = true;
                        break;
                    }
                }

                if (!shaderExists) {
                    CreateAndAddShader(shaderPath, shaderName);
                } else {
                    if (existingShadersReported.find(shaderName) == existingShadersReported.end()) {
                        std::cerr << "[ProjectExplorer] Shader " << shaderName << " already exists in AssetManager." << std::endl;
                        existingShadersReported.insert(shaderName);
                    }
                }

                clickedInsideSelectable = true;
            } catch (const std::runtime_error& e) {
                std::cerr << "[ProjectExplorer] Error creating or compiling shader: " << e.what() << std::endl;
                ImGui::OpenPopup("Shader Error");
            }
        }

        ImGui::EndPopup();
    }
}

void ProjectExplorer::displayShaders() {
    const auto& assetManagerShaders = AssetManager::getInstance().getShaders();

    if (!assetManagerShaders.empty()) {
        ImGui::Separator();
        ImGui::Text("Shaders (Loaded in AssetManager):");
        for (const auto& [uuidStr, shaderProgram] : assetManagerShaders) {
            if (userShaderUUIDs.find(uuidStr) == userShaderUUIDs.end()) {
                continue;
            }

            std::string displayName = shaderProgram->getName() + " [" + uuidStr + "]";

            bool isSelected = (SelectionManager::getInstance().getSelectedShader() == shaderProgram);

            ImGui::PushID(uuidStr.c_str());

            if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                SelectionManager::getInstance().selectShader(shaderProgram);
                clickedInsideSelectable = true;
                std::cout << "[ProjectExplorer] Selected shader: " << shaderProgram->getName() << " with UUID: " << uuidStr << std::endl;
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("SHADER_PAYLOAD", uuidStr.c_str(), uuidStr.size() + 1, ImGuiCond_Once);
                ImGui::Text("Dragging %s", shaderProgram->getName().c_str());
               // std::cout << "[ProjectExplorer] Initiated drag for shader: " << shaderProgram->getName()   << " with UUID: " << uuidStr << std::endl;

                ImGui::EndDragDropSource();
            }

            ImGui::PopID();
        }
    } else {
        std::cout << "[ProjectExplorer] No shaders loaded in AssetManager." << std::endl;
    }
}

void ProjectExplorer::CreateStandardShaderFile(const std::string& shaderPath) {
    std::ofstream shaderFile(shaderPath);
    if (!shaderFile.is_open()) {
        throw std::runtime_error("Failed to create shader file: " + shaderPath);
    }

    shaderFile << "#shader vertex\n"
              << "#version 330 core\n"
              << "layout(location = 0) in vec3 aPos;\n"
              << "layout(location = 1) in vec3 aColor;\n"
              << "layout(location = 2) in vec2 aTexCoord;\n\n"
              << "out vec3 vertexColor;\n"
              << "out vec2 texCoord;\n\n"
              << "uniform mat4 model;\n"
              << "uniform mat4 view;\n"
              << "uniform mat4 projection;\n\n"
              << "void main()\n"
              << "{\n"
              << "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
              << "    vertexColor = aColor;\n"
              << "    texCoord = aTexCoord;\n"
              << "}\n\n";

    shaderFile << "#shader fragment\n"
               << "#version 330 core\n"
               << "in vec3 vertexColor;\n"
               << "in vec2 texCoord;\n\n"
               << "out vec4 FragColor;\n\n"
               << "uniform sampler2D albedoTexture;\n"
               << "uniform vec3 baseColor;\n\n"
               << "void main()\n"
               << "{\n"
               << "    vec4 texColor = texture(albedoTexture, texCoord);\n"
               << "    FragColor = texColor * vec4(baseColor, 1.0) * vec4(vertexColor, 1.0);\n"
               << "}\n\n";

    shaderFile.close();

    std::cout << "Shader file being created at: " << shaderPath << std::endl;
}

void ProjectExplorer::EnsureAssetsFolderExists() {
    std::string assetsDir = std::string(SOURCE_DIR) + "/assets/shaders/";
    if (!std::filesystem::exists(assetsDir)) {
        std::filesystem::create_directories(assetsDir);
      //  std::cout << "[ProjectExplorer] Assets folder created at: " << assetsDir << std::endl;
    } else {
     //   std::cout << "[ProjectExplorer] Assets folder already exists at: " << assetsDir << std::endl;
    }
}