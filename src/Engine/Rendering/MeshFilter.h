//
// Created by Simeon on 9/27/2024.
//

#ifndef MESHFILTER_H
#define MESHFILTER_H

#include "../ECS/Component.h"
#include "Mesh.h"

namespace IDK::Components
{
    class MeshFilter final : public Component, public std::enable_shared_from_this<MeshFilter> {
    public:
        MeshFilter() : Component("MeshFilter") {}
        ~MeshFilter() override = default;

        const std::string & getName() const override {
            return name;
        }
        void setName(const std::string &newName) override {
            name = newName;
        }

        void setMesh(const std::shared_ptr<IDK::Graphics::Mesh>& mesh) {
            this->mesh = mesh;
            if(mesh) {
                name = mesh->getName();
            }
            // UpdateMesh();
        }

        const std::shared_ptr<IDK::Graphics::Mesh> &getMesh() const {
            return mesh;
        }
        void clearMesh() {}

        void UpdateMesh(){
            if (mesh) {
                mesh->SetupMesh();
            }
        }

    private:
        std::shared_ptr<IDK::Graphics::Mesh> mesh;
        std::string name;
    };
}

#endif //MESHFILTER_H
