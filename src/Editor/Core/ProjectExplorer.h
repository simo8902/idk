//
// Created by Simeon on 4/8/2024.
//

#ifndef CORE_PROJECTEXPLORER_H
#define CORE_PROJECTEXPLORER_H

#include <string>
#include <memory>
#include <unordered_set>
#include <vector>
#include "AssetItem.h"
#include "Entity.h"
#include "GameObject.h"
#include "imgui.h"
#include "glad/glad.h"


class File {
public:
    std::string Name;
    enum class FileType {
        Text,
        Image,
        Unknown,
        Assembly
    } Type;

    File(const std::string& name, FileType type) : Name(name), Type(type) {}
};


class Directory {
public:
    std::string Name;
    std::vector<Directory*> Directories;
    std::vector<File*> Files;

    Directory(const std::string& name) : Name(name) {}
    ~Directory() {
        for (auto* dir : Directories) delete dir;
        for (auto* file : Files) delete file;
    }
};

class String {
public:
    static std::string Sanitize(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (std::isalnum(c) || c == ' ' || c == '_' || c == '-') {
                result += c;
            }
        }
        return result;
    }
};

class ProjectExplorer {
public:
    ProjectExplorer();
    ~ProjectExplorer();

   // void DrawAssetFolder(const std::shared_ptr<AssetItem>& folder, int id);
   // void DrawAssetFile(const std::shared_ptr<AssetItem>& file, int id);

    void RenderAssetItem(const std::shared_ptr<AssetItem>& asset, float iconSize);
    void RenderGameObject(const std::shared_ptr<Entity>& entity, float iconSize);

    void DrawDirectory(Directory* dir, int index) {
        ImVec2 buttonSize = ImVec2(80, 80);
        if (ImGui::Button(dir->Name.c_str(), buttonSize)) {
            // Change directory
            m_CurrentDirectory = dir;
        }
    }

    void DrawFile(File* file, int index) {
        ImVec2 buttonSize = ImVec2(80, 80);
        ImGui::Button(file->Name.c_str(), buttonSize);
    }

    Directory* m_CurrentDirectory = nullptr;
    std::string m_SearchKeyword;

    void renderProjectExplorer();
private:
    bool printLog = false;

    void RenderContentArea(const std::shared_ptr<AssetItem>& folder);
    void RenderFolderTree(const std::shared_ptr<AssetItem>& folder);
    void RenderAssetItemAsIcon(const std::shared_ptr<AssetItem>& item, const float & iconSize);
    void HandleFolderPopups(const std::shared_ptr<AssetItem>& folder);
    bool createFolderPopupOpen = false;
    const char* GetAssetIcon(const AssetType & type);
    std::vector<std::string> SplitTextIntoLines(const std::string& text, float maxWidth, const ImFont* font);
    GLuint loadTexture(const std::string& filePath);

    /*
    bool caseInsensitiveFind(const std::string& str, const std::string& substr);
    std::string AbbreviateText(const std::string& text, float maxWidth, float fontScale );
    void handleContextMenu();
    void RenderAssetIcons(const std::vector<std::shared_ptr<AssetItem>>& assets);
    void RenderAssetItem(const std::shared_ptr<AssetItem>& asset);

    void RenderAssetIcon(const std::shared_ptr<AssetItem>& asset, const ImVec2& itemSize, float iconSize);
    void RenderFolderIcon(const std::shared_ptr<AssetItem>& folder, float iconSize);
    void HandleDragAndDrop(const std::shared_ptr<AssetItem>& asset);
    bool IsAssetSelected(const std::shared_ptr<AssetItem>& asset);
    void HandleSelection(const std::shared_ptr<AssetItem>& asset);
    void ShowContextMenu(const std::shared_ptr<AssetItem>& asset);
    void RenderIconAndText(const std::shared_ptr<AssetItem>& asset, const ImVec2& itemSize, float itemWidth) ;
    void HandleErrorPopups();
    void RenderDropTarget();*/

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
    GLuint shaderIconTexture = 0;
    bool m_showHiddenItems = false;
    float m_thumbnailSize = 64.0f;
    ImGuiTextFilter m_nameFilter;

    std::shared_ptr<AssetItem> targetFolderForCreation;
    std::unordered_set<std::string> existingShadersReported;
    std::unordered_set<std::string> existingMaterialsReported;

    /*
    void EnsureAssetsFolderExists();
    void loadShadersFromDirectory();
    bool shadersLoaded = false;
    void HandleCreationPopups();
    ImVec4 GetColorForAssetType(const AssetType type);*/

    std::unordered_set<std::string> userShaderUUIDs;
   // bool clickedInsideSelectable;
   // void CreateStandardShaderFile(const std::string& shaderPath);
   // void CreateAndAddShader(const std::string& shaderPath, const std::string& shaderName);
   // void displayShaders();
    void initializeTextures();
};


#endif