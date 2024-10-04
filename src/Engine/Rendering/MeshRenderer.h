//
// Created by Simeon on 9/27/2024.
//

#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <memory>

#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshFilter.h"
#include "Shader.h"

class MeshRenderer  : public Component{
public:
    MeshRenderer(const std::shared_ptr<MeshFilter>& meshFilter, const std::shared_ptr<Material>& material)
        : meshFilter(meshFilter), material(material) {}

    void Render() {
        if (meshFilter && meshFilter->getMesh() && material && material->getShader()) {
            material->getShader()->Use(); // Activate the shader program
            meshFilter->getMesh()->Draw(*material->getShader());
        } else {
            // Handle cases where meshFilter, mesh, material, or shader is missing
            // Optionally, log a warning or use a default shader
        }
    }

    std::unique_ptr<Component> clone() const override {
        return std::make_unique<MeshRenderer>(*this);
    }
    std::shared_ptr<Material> getMaterial() const {
        return material;
    }

    void setMaterial(const std::shared_ptr<Material>& newMaterial) {
        material = newMaterial;
    }


private:
    std::shared_ptr<MeshFilter> meshFilter;
    std::shared_ptr<Material> material;
};

#endif //MESHRENDERER_H
