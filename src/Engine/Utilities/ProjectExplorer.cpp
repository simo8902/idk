// ProjectExplorer.cpp
#include "ProjectExplorer.h"
#include "AssetManager.h"
#include "imgui.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <boost/uuid/uuid_io.hpp>
#include "IconsFontAwesome6Brands.h"

ProjectExplorer::ProjectExplorer()
    : folderIcon(ICON_FA_FOLDER),
      shaderIcon(ICON_FA_SHADER),
      materialIcon(ICON_FA_MATERIAL),
      selectedAsset(nullptr) {
    rootFolder = AssetManager::getInstance().getRootFolder();
}
ProjectExplorer::~ProjectExplorer() {
}

bool ProjectExplorer::caseInsensitiveFind(const std::string& str, const std::string& substr) {
    if (substr.empty()) return true;
    auto it = std::search(
        str.begin(), str.end(),
        substr.begin(), substr.end(),
        [](char ch1, char ch2) { return std::tolower(static_cast<unsigned char>(ch1)) == std::tolower(static_cast<unsigned char>(ch2)); }
    );
    return (it != str.end());
}

void ProjectExplorer::renderProjectExplorer() {
    ImGui::Begin("Project Explorer");

    // Calculate dimensions
    float windowWidth = ImGui::GetContentRegionAvail().x;
    float windowHeight = ImGui::GetContentRegionAvail().y;
    float leftPanelWidth = windowWidth * 0.2f; // 20% for the folder tree

    // Left Panel: Folder Tree
    ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, windowHeight), true);
    {
        std::lock_guard<std::mutex> lock(AssetManager::getInstance().assetMutex);
        RenderFolderTree(rootFolder);
    }
    ImGui::EndChild();

    ImGui::SameLine(); // Place the next widget on the same line

    // Right Panel: Content Area
    ImGui::BeginChild("ContentArea", ImVec2(0, windowHeight), true);
    {
        std::lock_guard<std::mutex> lock(AssetManager::getInstance().assetMutex);
        if (selectedFolder) {
            RenderContentArea(selectedFolder);
        } else {
            RenderContentArea(rootFolder);
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void ProjectExplorer::RenderFolderTree(const std::shared_ptr<AssetItem>& folder) {
    if (!folder || folder->getType() != AssetType::Folder) return;

    ImGui::PushID(folder->getUUID().data);

    std::string nodeLabel = std::string(folderIcon) + " " + folder->getName();

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (selectedFolder == folder) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(), nodeFlags);

    if (ImGui::IsItemClicked()) {
        selectedFolder = folder;
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

    const int iconsPerRow = 5;
    float iconSize = 64.0f;
    int iconCount = 0;

    for (const auto& item : folder->getChildren()) {
        RenderAssetItemAsIcon(item, iconSize);
        iconCount++;

        if (iconCount % iconsPerRow != 0) {
            ImGui::SameLine();
        }
    }
}
void ProjectExplorer::RenderAssetItem(const std::shared_ptr<AssetItem>& asset) {
    ImGui::PushID(asset->getUUID().data);

    // Display asset with icon
    const char* icon = GetAssetIcon(asset->getType());
    std::string itemLabel = std::string("    ") + icon + " " + asset->getName();

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (selectedAsset == asset) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::TreeNodeEx(itemLabel.c_str(), nodeFlags);

    // Handle asset selection
    if (ImGui::IsItemClicked()) {
        selectedAsset = asset;
    }

    ImGui::PopID();
}
void ProjectExplorer::HandleFolderPopups(const std::shared_ptr<AssetItem>& folder) {
    if (createFolderPopupOpen) {
        ImGui::OpenPopup("Create New Folder");
        createFolderPopupOpen = false;
    }
    if (ImGui::BeginPopupModal("Create New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char newFolderName[256] = "";
        ImGui::InputText("Folder Name", newFolderName, IM_ARRAYSIZE(newFolderName));
        if (ImGui::Button("Create")) {
            /*
            // Create the folder
            AssetManager::getInstance().createFolder(folder, newFolderName);
            newFolderName[0] = '\0'; // Reset input
            ImGui::CloseCurrentPopup();*/
        }
        ImGui::SameLine();
        /*
        if (ImGui::Button("Cancel")) {
            newFolderName[0] = '\0'; // Reset input
            ImGui::CloseCurrentPopup();}*/

        ImGui::EndPopup();
    }

    // Similar implementations for renameFolderPopupOpen and deleteFolderPopupOpen
}

void ProjectExplorer::RenderAssetIcons(const std::vector<std::shared_ptr<AssetItem>>& items) {
    const int iconsPerRow = 5;
    int iconCount = 0;

    for (const auto& item : items) {
        if (item->getType() == AssetType::Folder) continue; // Folders are handled separately

        RenderAssetItemAsIcon(item, 64.0f);
        iconCount++;

        if (iconCount % iconsPerRow != 0) {
            ImGui::SameLine();
        }
    }
}

void ProjectExplorer::RenderAssetItemAsIcon(const std::shared_ptr<AssetItem>& items, float iconSize) {
    if (!items) return;

    const char* icon = nullptr;
    if (items->getType() == AssetType::Folder) {
        icon = folderIcon.c_str();
    } else {
        icon = GetAssetIcon(items->getType());
    }

    if (!icon || strlen(icon) == 0) {
        icon = "[Unknown]";
    }

    ImGui::BeginGroup();

    std::string iconLabel = std::string(icon) + "###" + boost::uuids::to_string(items->getUUID());

    if (ImGui::Button(iconLabel.c_str(), ImVec2(iconSize, iconSize))) {
        if (items->getType() == AssetType::Folder) {
            selectedFolder = std::static_pointer_cast<AssetItem>(items);
        } else {
            selectedAsset = items;
        }
    }

    float textWidth = ImGui::CalcTextSize(items->getName().c_str()).x;
    float textOffset = (iconSize - textWidth) / 2.0f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
    ImGui::TextWrapped("%s", items->getName().c_str());

    ImGui::EndGroup();
}
const char* ProjectExplorer::GetAssetIcon(AssetType type) {
    switch (type) {
        case AssetType::Shader:
            return shaderIcon.c_str();
        case AssetType::PredefinedShader:
            return shaderIcon.c_str();
        case AssetType::Material:
            return materialIcon.c_str();
        default:
            return "[Asset]";
    }
}