#include "ProjectExplorer.h"
#include "AssetManager.h"
#include "SelectionManager.h"
#include "imgui.h"
#include <algorithm>
#include <thread>
#include "IconsFontAwesome6Brands.h"
#include "DragAndDropPayload.h"
#include <string>
#include <iostream>
#include <boost/uuid/uuid_io.hpp> // For boost::uuids::to_string

ProjectExplorer::ProjectExplorer()
    : folderIcon(ICON_FA_FOLDER),
      shaderIcon(ICON_FA_SHADER),
      materialIcon(ICON_FA_MATERIAL),
      shadersLoaded(false),
      clickedInsideSelectable(false) {
    rootFolder = AssetManager::getInstance().getRootFolder();
}

ProjectExplorer::~ProjectExplorer() {}

void ProjectExplorer::renderProjectExplorer() {
    ImGui::Begin("Project Explorer");

    if (!shadersLoaded) {
        loadShadersFromDirectory();
        shadersLoaded = true;
    }

    float windowWidth = ImGui::GetContentRegionAvail().x;
    float windowHeight = ImGui::GetContentRegionAvail().y;
    float leftPanelWidth = windowWidth * 0.2f;

    ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, windowHeight), true);
    {
        std::lock_guard<std::mutex> lock(AssetManager::getInstance().assetMutex);
        RenderFolderTree(rootFolder);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ContentArea", ImVec2(0, windowHeight), true);
    {
        std::lock_guard<std::mutex> lock(AssetManager::getInstance().assetMutex);
        auto selectedFolder = SelectionManager::getInstance().getSelectedFolder();
        auto folderToRender = selectedFolder ? selectedFolder : rootFolder;
        RenderContentArea(folderToRender);
    }
    ImGui::EndChild();

    ImGui::End();

    HandleCreationPopups();

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
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid payload received.");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            std::cout << "[ProjectExplorer] Closed 'Invalid Payload' popup." << std::endl;
        }
        ImGui::EndPopup();
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !clickedInsideSelectable && ImGui::IsWindowHovered()) {
        SelectionManager::getInstance().clearSelection();
    }

    clickedInsideSelectable = false;
}

void ProjectExplorer::RenderFolderTree(const std::shared_ptr<AssetItem>& folder) {
    if (!folder || folder->getType() != AssetType::Folder) return;

    if (folder->getUUIDStr().empty()) {
        std::cerr << "[ProjectExplorer] Folder has empty UUID: " << folder->getName() << std::endl;
        return;
    }

    ImGui::PushID(folder->getUUIDStr().c_str());

    std::string nodeLabel = std::string(folderIcon) + " " + folder->getName();

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (SelectionManager::getInstance().getSelectedFolder() == folder) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(), nodeFlags);

    if (ImGui::IsItemClicked()) {
        SelectionManager::getInstance().selectFolder(folder);
        clickedInsideSelectable = true;
    }

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Create Shader")) {
            createShaderPopupOpen = true;
            targetFolderForCreation = folder;
        }
        if (ImGui::MenuItem("Create New Folder")) {
            createFolderPopupOpen = false;
            HandleFolderPopups(folder);
        }
        if (ImGui::MenuItem("Create Material")) {
            createMaterialPopupOpen = true;
            targetFolderForCreation = folder;
        }
        ImGui::EndPopup();
    }

    if (nodeOpen) {
        for (const auto& child : folder->getChildren()) {
            if (child->getType() == AssetType::Folder) {
                RenderFolderTree(child);
            }
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void ProjectExplorer::RenderContentArea(const std::shared_ptr<AssetItem>& folder) {
    if (!folder) return;

    const float iconSize = 32.0f;
    int itemsPerRow = 5;
    int itemCount = 0;

    ImGui::BeginChild("ProjectExplorerContent", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& item : folder->getChildren()) {
        RenderAssetItemAsIcon(item, iconSize);
        itemCount++;

        if (itemCount % itemsPerRow != 0) {
            ImGui::SameLine();
        }
    }

    ImGui::EndChild();
}

void ProjectExplorer::RenderAssetItemAsIcon(const std::shared_ptr<AssetItem>& item, const float & iconSize) {
    if (!item) return;

    const char* icon = GetAssetIcon(item->getType());
    if (!icon || strlen(icon) == 0) {
        icon = "[Unknown]";
    }

    ImGui::BeginGroup();

    const std::string uniqueID = "##" + boost::uuids::to_string(item->getUUID());
    const std::string buttonLabel = std::string(icon) + uniqueID;

    const float buttonSizeMultiplier = 1.25f;
    const ImVec2 buttonSize(iconSize * buttonSizeMultiplier, iconSize * buttonSizeMultiplier);

    bool isSelected = (selectedAsset == item || selectedFolder == item);

  //  const bool isSelected = SelectionManager::getInstance().isItemSelected(item);

    if (ImGui::Button(buttonLabel.c_str(), buttonSize)) {
        clickedInsideSelectable = true;
        SelectionManager::getInstance().toggleSelectItem(item);

        switch (item->getType()) {
            case AssetType::Folder:
                SelectionManager::getInstance().selectFolder(item);
            break;
            case AssetType::Shader:
                SelectionManager::getInstance().selectShader(std::static_pointer_cast<Shader>(item));
            break;
            case AssetType::Material:
                SelectionManager::getInstance().selectMaterial(std::static_pointer_cast<Material>(item));
            break;
            default:
                break;
        }
    }

    if (isSelected) {
        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 itemMax = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, ImGui::GetColorU32(ImGuiCol_FrameBgHovered));
    }

    // Handle hover effects
    if (ImGui::IsItemHovered()) {
        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 itemMax = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRect(itemMin, itemMax, ImGui::GetColorU32(ImGuiCol_ButtonHovered), 4.0f);
    }



    if (item->getType() == AssetType::Shader) {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const std::string uuidStr = boost::uuids::to_string(item->getUUID());

            ImGui::SetDragDropPayload(PAYLOAD_SHADER, uuidStr.c_str(), sizeof(char) * (uuidStr.size() + 1)); // Include null terminator
            ImGui::Text("Dragging %s", item->getName().c_str());

            ImGui::EndDragDropSource();
        }
    }
    else if (item->getType() == AssetType::Material) {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const std::string uuidStr = boost::uuids::to_string(item->getUUID());

            ImGui::SetDragDropPayload(PAYLOAD_MATERIAL, uuidStr.c_str(), sizeof(char) * (uuidStr.size() + 1));
            ImGui::Text("Dragging %s", item->getName().c_str());

            ImGui::EndDragDropSource();
        }
    }

    float textWidth = ImGui::CalcTextSize(item->getName().c_str()).x;
    float maxTextWidth = iconSize * 1.5f;
    if (textWidth > maxTextWidth) {
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + maxTextWidth);
        ImGui::TextWrapped("%s", item->getName().c_str());
        ImGui::PopTextWrapPos();
    } else {
        float textOffset = (iconSize - textWidth) / 2.0f;
        if (textOffset > 0) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
        }
        ImGui::Text("%s", item->getName().c_str());
    }

    ImGui::EndGroup();
}


void ProjectExplorer::HandleFolderPopups(const std::shared_ptr<AssetItem>& folder) {
    if (ImGui::BeginPopupModal("Create New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char newFolderName[256] = "";
        ImGui::InputText("Folder Name", newFolderName, IM_ARRAYSIZE(newFolderName));
        if (ImGui::Button("Create")) {

            AssetManager::getInstance().createFolder(folder, newFolderName);
            newFolderName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            newFolderName[0] = '\0';
            ImGui::CloseCurrentPopup();}

        ImGui::EndPopup();
    }

    // TODO: renameFolderPopupOpen and deleteFolderPopupOpen
}

const char* ProjectExplorer::GetAssetIcon(const AssetType & type) {
    switch (type) {
        case AssetType::Shader:
            return shaderIcon.c_str();
        case AssetType::PredefinedShader:
            return shaderIcon.c_str();
        case AssetType::Material:
            return materialIcon.c_str();
        case AssetType::Folder:
            return folderIcon.c_str();
        default:
            return "[Asset]";
    }
}

void ProjectExplorer::HandleCreationPopups() {
    if (createShaderPopupOpen) {
        ImGui::OpenPopup("Create New Shader");
        createShaderPopupOpen = false;
    }
    if (ImGui::BeginPopupModal("Create New Shader", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char shaderName[256] = "";
        ImGui::InputText("Shader Name", shaderName, IM_ARRAYSIZE(shaderName));
        if (ImGui::Button("Create")) {
            if (targetFolderForCreation && strlen(shaderName) > 0) {
                std::lock_guard lock(AssetManager::getInstance().assetMutex);

                std::string shaderBaseName = shaderName;
                std::string shaderDir = targetFolderForCreation->getPath();

                std::string shaderPath = shaderDir + "/" + shaderBaseName + ".glsl";

                try {
                    std::cout << "[ProjectExplorer] Creating shader file at: " << shaderPath << std::endl;
                    CreateStandardShaderFile(shaderPath);

                    // Check if shader already exists
                    /*
                    if (!AssetManager::getInstance().getShaderByName(shaderBaseName)) {
                        CreateAndAddShader(shaderPath, shaderBaseName);
                    } else {
                        std::cerr << "[ProjectExplorer] Shader " << shaderBaseName << " already exists in AssetManager." << std::endl;
                    }*/
                } catch (const std::runtime_error& e) {
                    std::cerr << "[ProjectExplorer] Error creating or compiling shader: " << e.what() << std::endl;
                    ImGui::OpenPopup("Shader Error");
                }
                shaderName[0] = '\0';
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            shaderName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (createMaterialPopupOpen) {
        ImGui::OpenPopup("Create New Material");
        createMaterialPopupOpen = false;
    }
    if (ImGui::BeginPopupModal("Create New Material", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char materialName[256] = "";
        ImGui::InputText("Material Name", materialName, IM_ARRAYSIZE(materialName));
        if (ImGui::Button("Create")) {
            if (targetFolderForCreation && strlen(materialName) > 0) {
                std::cout << "[ProjectExplorer] Creating material in folder: " << targetFolderForCreation->getName() << std::endl;

                std::lock_guard<std::mutex> lock(AssetManager::getInstance().assetMutex);
                AssetManager::getInstance().createMaterial(targetFolderForCreation, materialName);
                std::cout << "[ProjectExplorer] Created material with name: " << materialName << std::endl;
                materialName[0] = '\0';
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            materialName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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

                      //  userShaderUUIDs.insert(newShader->getUUID());

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
