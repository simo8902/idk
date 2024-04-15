//
// Created by Simeon on 4/15/2024.
//

#ifndef LUPUSFIRE_CORE_RENDERINGMANAGEMENT_H
#define LUPUSFIRE_CORE_RENDERINGMANAGEMENT_H


class Renderer;
class SceneLoader;

class RenderingManagement {
public:
    Renderer* renderer;
    SceneLoader* sceneLoader;

    RenderingManagement();
    ~RenderingManagement();

};


#endif //LUPUSFIRE_CORE_RENDERINGMANAGEMENT_H
