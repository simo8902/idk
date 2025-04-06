//
// Created by Simeon on 9/27/2024.
//

#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "../ECS/Component.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "Shader.h"

namespace IDK::Components
{
    class MeshRenderer final : public Component{
    public:
        MeshRenderer() : Component("MeshRenderer") {}

        explicit MeshRenderer(const std::shared_ptr<IDK::Components::MeshFilter>& filter)
            : Component("MeshRenderer"), meshFilter(filter) {}

        void setMeshFilter(const std::shared_ptr<IDK::Components::MeshFilter>& filter) {
            meshFilter = filter;
        }

        void Render(const IDK::Graphics::Shader* shader) const
        {
            if (!meshFilter || !meshFilter->getMesh()) {
                std::cerr << "[MeshRenderer] Cannot render: missing mesh filter, mesh or shader.\n";
                return;
            }

            meshFilter->getMesh()->Draw(*shader);
        }
    private:
        std::shared_ptr<IDK::Components::MeshFilter> meshFilter;
    };
}

#endif //MESHRENDERER_H
