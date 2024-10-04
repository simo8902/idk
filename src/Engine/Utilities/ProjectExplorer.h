//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_PROJECTEXPLORER_H
#define LUPUSFIRE_CORE_PROJECTEXPLORER_H

#include <vector>
#include <memory>
#include <string>
#include "Mesh.h"
#include "Material.h"

class ProjectExplorer {
public:
    ProjectExplorer();

    void renderProjectExplorer();
    void CreateStandardShaderFile(const std::string& shaderPath);
    void EnsureAssetsFolderExists();

private:
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Material>> materials;

    std::shared_ptr<Mesh> selectedMesh = nullptr;
    std::shared_ptr<Material> selectedMaterial = nullptr;

};


#endif //NAV2SFM Core_PROJECT_DIR_H
