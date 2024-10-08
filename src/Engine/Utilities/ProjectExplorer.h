//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_PROJECTEXPLORER_H
#define LUPUSFIRE_CORE_PROJECTEXPLORER_H

#include <string>
#include <memory>
#include <vector>
#include "AssetItem.h"

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
    void RenderAssetItemAsIcon(const std::shared_ptr<AssetItem>& asset, float iconSize);
    void HandleFolderPopups(const std::shared_ptr<AssetItem>& folder);
    bool createFolderPopupOpen = false;
    const char* GetAssetIcon(AssetType type);

    std::string folderIcon;
    std::string shaderIcon;
    std::string materialIcon;

    std::shared_ptr<AssetItem> rootFolder;
    std::shared_ptr<AssetItem> selectedFolder;
    std::shared_ptr<AssetItem> selectedAsset;
};


#endif //NAV2SFM Core_PROJECT_DIR_H
