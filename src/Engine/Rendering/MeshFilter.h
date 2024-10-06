//
// Created by Simeon on 9/27/2024.
//

#ifndef MESHFILTER_H
#define MESHFILTER_H

#include "Component.h"
#include "Logger.h"
#include "Mesh.h"

class MeshFilter : public Component {
public:
    MeshFilter() = default;

    MeshFilter(const std::shared_ptr<GameObject> & owner) : mesh(nullptr), m_owner(owner) {}
    ~MeshFilter() override {}

    void setMesh(const std::shared_ptr<Mesh>& mesh) {
        this->mesh = mesh;
    }

    std::shared_ptr<Mesh> getMesh() const {
        return mesh;
    }

    std::unique_ptr<Component> clone() const override {
        return std::make_unique<MeshFilter>(*this);
    }

    void clearMesh() {
        if (mesh) {
            logger.Log("[DEBUG] Mesh is valid, proceeding to clear.\n");
            mesh.reset();
            logger.Log( "[DEBUG] Cleared mesh from MeshFilter.\n");
        } else {
            logger.Log("[DEBUG] No mesh to clear.\n");
        }
    }

    void UpdateMesh() {
        if (mesh) {
            mesh->SetupMesh();
        }
    }
    std::shared_ptr<Mesh> mesh;

private:
    std::weak_ptr<GameObject> m_owner;
};

#endif //MESHFILTER_H
