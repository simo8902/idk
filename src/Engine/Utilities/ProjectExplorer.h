//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_PROJECTEXPLORER_H
#define LUPUSFIRE_CORE_PROJECTEXPLORER_H

#include <vector>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>

#include "Material.h"

class ProjectExplorer {
public:
    ProjectExplorer();

    void renderProjectExplorer();
    void CreateStandardShaderFile(const std::string& shaderPath);
    void EnsureAssetsFolderExists();
    void CreateAndAddShader(const std::string& shaderPath, const std::string& shaderName);

private:
    bool shadersLoaded = false;
    std::unordered_set<std::string> existingShadersReported;
    std::unordered_set<std::string> existingMaterialsReported;
    bool clickedInsideSelectable;
    std::unordered_set<std::string> userShaderUUIDs;

    void loadShadersFromDirectory();
    void handleContextMenu();
    void displayShaders();
    void displayMaterials();

    std::vector<std::shared_ptr<Material>> materials;

    std::shared_ptr<Material> selectedMaterial = nullptr;
    std::set<std::string> shadersCreatedByContextMenu;

};


#endif //NAV2SFM Core_PROJECT_DIR_H
