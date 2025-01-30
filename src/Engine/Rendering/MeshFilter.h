//
// Created by Simeon on 9/27/2024.
//

#ifndef MESHFILTER_H
#define MESHFILTER_H

#include "Component.h"
#include "Logger.h"
#include "Mesh.h"

class MeshFilter final : public Component {
public:
    explicit MeshFilter(const std::string & name) : name(name){}
    ~MeshFilter() override = default;

    const std::string & getName() const override { return name; }
    void setName(const std::string &newName) override {
        name = newName;
    }

    void setMesh(const std::shared_ptr<Mesh>& mesh) {
        this->mesh = mesh;
        UpdateMesh();
    }

    std::shared_ptr<Mesh> getMesh() const {
        return mesh;
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

    void UpdateMesh() const{
        if (mesh) {
            mesh->SetupMesh();
        }
    }
    std::shared_ptr<Mesh> mesh;

private:
    std::string name;
    std::weak_ptr<GameObject> m_owner;
};

#endif //MESHFILTER_H
