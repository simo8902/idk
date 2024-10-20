//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_PROJECTEXPLORER_H
#define LUPUSFIRE_CORE_PROJECTEXPLORER_H

#include <string>
#include <memory>
#include <unordered_set>
#include <vector>
#include "AssetItem.h"
#include "imgui.h"

class ProjectExplorer {
public:
    ProjectExplorer();
    ~ProjectExplorer();

    void renderProjectExplorer();
    void RenderAssetItem(const std::shared_ptr<AssetItem>& asset);
    void RenderContentArea(const std::shared_ptr<AssetItem>& folder);
private:
    bool caseInsensitiveFind(const std::string& str, const std::string& substr);
    void RenderFolderTree(const std::shared_ptr<AssetItem>& folder);
    void RenderAssetIcons(const std::vector<std::shared_ptr<AssetItem>>& assets);
    void RenderAssetItemAsIcon(const std::shared_ptr<AssetItem>& item, const float & iconSize);
    void HandleFolderPopups(const std::shared_ptr<AssetItem>& folder);
    bool createFolderPopupOpen = false;
    const char* GetAssetIcon(const AssetType & type);
    std::string AbbreviateText(const std::string& text, float maxWidth, float fontScale );
    void handleContextMenu();

    void RenderAssetIcon(const std::shared_ptr<AssetItem>& asset, const ImVec2& itemSize, float iconSize);
    void RenderFolderIcon(const std::shared_ptr<AssetItem>& folder, float iconSize);
    void HandleDragAndDrop(const std::shared_ptr<AssetItem>& asset);
    bool IsAssetSelected(const std::shared_ptr<AssetItem>& asset);
    void HandleSelection(const std::shared_ptr<AssetItem>& asset);
    void ShowContextMenu(const std::shared_ptr<AssetItem>& asset);
    void RenderIconAndText(const std::shared_ptr<AssetItem>& asset, const ImVec2& itemSize, float itemWidth) ;
    void HandleErrorPopups();
    void RenderDropTarget();
    std::string folderIcon;
    std::string shaderIcon;
    std::string materialIcon;

    std::shared_ptr<AssetItem> rootFolder;
    std::shared_ptr<AssetItem> selectedFolder;
    std::shared_ptr<AssetItem> selectedAsset;
    std::shared_ptr<AssetItem> selectedShader;
    std::shared_ptr<AssetItem> selectedMaterial;

    bool createShaderPopupOpen = false;
    bool createMaterialPopupOpen = false;

    std::shared_ptr<AssetItem> targetFolderForCreation;
    std::unordered_set<std::string> existingShadersReported;
    std::unordered_set<std::string> existingMaterialsReported;
    void EnsureAssetsFolderExists();
    void loadShadersFromDirectory();
    bool shadersLoaded = false;
    void HandleCreationPopups();
    ImVec4 GetColorForAssetType(const AssetType type);

    std::unordered_set<std::string> userShaderUUIDs;
    bool clickedInsideSelectable;
    void CreateStandardShaderFile(const std::string& shaderPath);
    void CreateAndAddShader(const std::string& shaderPath, const std::string& shaderName);
    void displayShaders();
};


#endif //NAV2SFM Core_PROJECT_DIR_H
