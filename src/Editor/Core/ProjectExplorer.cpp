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
#include <boost/uuid/uuid_io.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


GLuint loadTexture(const std::string& filePath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

ProjectExplorer::ProjectExplorer()
    : folderIcon("[Folder]"),
      shaderIcon("[Shader]"),
      materialIcon("[Material]"),
      shadersLoaded(false),
      clickedInsideSelectable(false) {

}

ProjectExplorer::~ProjectExplorer() = default;

void ProjectExplorer::renderProjectExplorer() {
    ImGui::Begin("Project Explorer");
    auto sharedRootFolder = AssetManager::getInstance().getRootFolder();

    if (!shadersLoaded) {
       // loadShadersFromDirectory();
        shadersLoaded = true;
    }

    float windowWidth = ImGui::GetContentRegionAvail().x;
    float windowHeight = ImGui::GetContentRegionAvail().y;
    float leftPanelWidth = windowWidth * 0.2f;

    ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, windowHeight), true);
    {
        //std::lock_guard<std::mutex> lock(AssetManager::getInstance().assetMutex);
        if (sharedRootFolder) {
            rootFolder = sharedRootFolder;
            RenderFolderTree(sharedRootFolder);
        } else {
            ImGui::Text("Root folder is no longer valid.");
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ContentArea", ImVec2(0, windowHeight), true);
    {
      //  std::lock_guard<std::mutex> lock(AssetManager::getInstance().assetMutex);
        auto selectedFolder = SelectionManager::getInstance().getSelectedFolder();
        auto folderToRender = selectedFolder ? selectedFolder : sharedRootFolder;

        if (!folderToRender) {
            ImGui::Text("No folder to render.");
        } else {
            RenderContentArea(folderToRender);
        }

    }
    ImGui::EndChild();
    ImGui::End();

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

    if (folderIcon.empty()) {
        folderIcon = "[Folder]";
    }

    std::string folderName = folder->getName().empty() ? "[Unnamed Folder]" : folder->getName();

    std::string nodeLabel = folderIcon + " " + folderName;

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
std::vector<std::string> ProjectExplorer::SplitTextIntoLines(const std::string& text, float maxWidth, const ImFont* font) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word, currentLine;

    const ImFont* usedFont = font ? font : ImGui::GetFont();

    while (iss >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        float lineWidth = usedFont->CalcTextSizeA(usedFont->FontSize, FLT_MAX, 0.0f, testLine.c_str()).x;

        if (lineWidth > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}
void ProjectExplorer::initializeTextures() {
    shaderIconTexture = loadTexture(SOURCE_DIR "/src/data/gui/shaderIcon.png");
    if (shaderIconTexture == 0) {
        std::cerr << "Failed to load shader icon texture!" << std::endl;
    }
}


void ProjectExplorer::RenderContentArea(const std::shared_ptr<AssetItem>& folder) {
    if (!folder || shaderIconTexture == 0) return;

    int maxVisibleItems = 50;
    int currentVisibleCount = 0;

    const float iconSize = 48.0f;
    const float padding = 10.0f;
    const int itemsPerRow = 7;
    int itemCount = 0;

    const auto& shaderMap = AssetManager::getInstance().getShaders();
    if (shaderMap.empty()) {
        ImGui::Text("No shaders available.");
        return;
    }

    ImGui::Text("Shaders:");
    ImGui::Separator();
    if (shaderIconTexture == 0) return;

    ImGui::BeginChild("ProjectExplorerContent", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    int id = 0;

    for (const auto& [shaderName, shader] : shaderMap) {
        if (!shader || currentVisibleCount >= maxVisibleItems) continue;
        currentVisibleCount++;

        ImGui::PushID(id++);

        ImGui::BeginGroup();

        ImGui::Image(reinterpret_cast<ImTextureID>(shaderIconTexture), ImVec2(iconSize, iconSize));

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + iconSize);

        ImGui::TextWrapped("%s", shaderName.c_str());

        ImGui::PopTextWrapPos();

        ImGui::EndGroup();

        itemCount++;
        if (itemCount % itemsPerRow != 0) {
            ImGui::SameLine(0.0f, padding);
        }

        ImGui::PopID();
    }


    ImGui::EndChild();
}

void ProjectExplorer::RenderAssetItemAsIcon(const std::shared_ptr<AssetItem>& item, const float & iconSize) {
    if (!item) return;

    const char* icon = GetAssetIcon(item->getType());
    // Ensure icon is not empty
    if (!icon || strlen(icon) == 0) {
        icon = "[Unknown]";
    }

    ImGui::BeginGroup();


    /*
    std::string prefix;
    if (item->getType() == AssetType::Material) {
        
        auto material = std::dynamic_pointer_cast<Material>(item);
        if (material) {
            prefix = material->isPredefinedMaterial() ? "[Predefined] " : "[File-based] ";
        }
    }

   // const std::string uniqueID = boost::uuids::to_string(item->getUUID());
    const std::string buttonLabel = prefix + icon + "##" + uniqueID;

    const float buttonSizeMultiplier = 1.25f;
    const ImVec2 buttonSize(iconSize * buttonSizeMultiplier, iconSize * buttonSizeMultiplier);

    bool isSelected = SelectionManager::getInstance().isItemSelected(item);


    if (ImGui::Button(buttonLabel.c_str(), buttonSize)) {
        clickedInsideSelectable = true;
        SelectionManager::getInstance().toggleSelectItem(item);

        switch (item->getType()) {
            case AssetType::Folder:
                SelectionManager::getInstance().selectFolder(item);
            break;
            case AssetType::PredefinedShader:
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

    if (item->getType() == AssetType::Material) {
        
        auto material = std::dynamic_pointer_cast<Material>(item);
        if (material) {
            std::string materialUniqueID = material->getUUIDStr();

            // Validate materialUniqueID
            if (materialUniqueID.empty()) {
                std::cerr << "[RenderAssetItemAsIcon] Warning: materialUniqueID is empty for material: " << material->getName() << std::endl;
                materialUniqueID = "invalid_material_uuid";
            }

            
            // Display material status
            if (material->isPredefinedMaterial()) {
                ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.0f, 1.0f), "Predefined Material");
            } else {
                ImGui::Text("File-based Material");
            }
        }


        // Drag and drop handling for materials
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const std::string uuidStr = boost::uuids::to_string(item->getUUID());
            ImGui::SetDragDropPayload(PAYLOAD_MATERIAL, uuidStr.c_str(), sizeof(char) * (uuidStr.size() + 1));
            ImGui::Text("Dragging %s", item->getName().c_str());
            ImGui::EndDragDropSource();
        }
    
    }else if (item->getType() == AssetType::Shader) {
        // Drag and drop handling for shaders
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            const std::string uuidStr = boost::uuids::to_string(item->getUUID());
            ImGui::SetDragDropPayload(PAYLOAD_SHADER, uuidStr.c_str(), sizeof(char) * (uuidStr.size() + 1));
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
    */
    ImGui::EndGroup();
}

void ProjectExplorer::HandleFolderPopups(const std::shared_ptr<AssetItem>& folder) {
    if (ImGui::BeginPopupModal("Create New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char newFolderName[256] = "";
        ImGui::InputText("Folder Name", newFolderName, IM_ARRAYSIZE(newFolderName));
        if (ImGui::Button("Create")) {

           // AssetManager::getInstance().createFolder(folder, newFolderName);
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
        case AssetType::RuntimeShader:
            return !shaderIcon.empty() ? shaderIcon.c_str() : "[Shader]";
        case AssetType::PredefinedShader:
            return !shaderIcon.empty() ? shaderIcon.c_str() : "[Shader]";
        case AssetType::Material:
            return !materialIcon.empty() ? materialIcon.c_str() : "[Material]";
        case AssetType::Folder:
            return !folderIcon.empty() ? folderIcon.c_str() : "[Folder]";
        default:
            return "[Asset]";
    }
}
