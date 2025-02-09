//
// Created by Simeon on 9/27/2024.
//

#ifndef MESHFILTER_H
#define MESHFILTER_H

#include "Component.h"
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
    void clearMesh() {}

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
