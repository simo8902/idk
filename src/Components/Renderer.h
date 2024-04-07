//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_RENDERER_H
#define LUPUSFIRE_CORE_RENDERER_H

#include "../Scene.h"
#include "../Shader.h"

class Renderer {
public:
    void initialize();
    void shutdown();
    void render(const Scene& scene);

private:
    Shader m_defaultShader;
};


#endif //LUPUSFIRE_CORE_RENDERER_H
