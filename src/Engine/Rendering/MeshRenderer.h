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
#include "Transform.h"

class MeshRenderer  : public Component{
public:
    MeshRenderer(const std::shared_ptr<GameObject>& owner, const std::shared_ptr<MeshFilter>& meshFilter, const std::shared_ptr<Material>& material)
        : meshFilter(meshFilter), material(material), gameObject(owner) {}

    void Render(const glm::mat4& view, const glm::mat4& projection) {
        if (!meshFilter || !meshFilter->getMesh() || !material || !material->getShader()) {
            std::cerr << "[MeshRenderer] Cannot render: missing mesh filter, mesh, material, or shader.\n";
            return;
        }

        std::shared_ptr<Shader> shader = material->getShader();
        shader->Use();

        auto transform = getGameObject()->getComponent<Transform>();
        glm::mat4 model = transform->getModelMatrix();

        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        meshFilter->getMesh()->Draw(*shader);

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
    std::shared_ptr<GameObject> getGameObject() const {
        return gameObject;
    }

private:
    std::shared_ptr<MeshFilter> meshFilter;
    std::shared_ptr<Material> material;
    std::shared_ptr<GameObject> gameObject;

};

#endif //MESHRENDERER_H
