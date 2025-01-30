//
// Created by Simeon on 9/27/2024.
//

#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Mesh.h"
#include "MeshFilter.h"
#include "Shader.h"

class MeshRenderer final : public Component{
public:
    explicit MeshRenderer(const std::shared_ptr<MeshFilter> &meshFilter)
         : meshFilter(meshFilter) {
    }

    void Render(const Shader* shader) const
    {
        if (!meshFilter || !meshFilter->getMesh()) {
            std::cerr << "[MeshRenderer] Cannot render: missing mesh filter, mesh or shader.\n";
            return;
        }

        meshFilter->getMesh()->Draw(*shader);
    }
private:
    std::shared_ptr<MeshFilter> meshFilter;
};

#endif //MESHRENDERER_H
