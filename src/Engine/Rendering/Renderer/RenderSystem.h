//
// Created by SIMEON on 4/5/2025.
//

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Renderer.h"

class RenderSystem {
public:
    explicit RenderSystem(std::shared_ptr<IDK::Renderer> renderer)
        : renderer(renderer) {}

    void render() {
        renderer->render();
    }

private:
    std::shared_ptr<IDK::Renderer> renderer;
};

#endif //RENDERSYSTEM_H
