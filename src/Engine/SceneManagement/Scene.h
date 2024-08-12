//
// Created by Simeon on 4/5/2024.
//

#ifndef LUPUSFIRE_CORE_SCENE_H
#define LUPUSFIRE_CORE_SCENE_H
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

class Scene {
public:
    Scene(Shader* shaderProgram, Shader* wireframem, const std::shared_ptr<Camera> & camera);
    ~Scene();

    static  std::vector<std::shared_ptr<GameObject>> objects;
    void Render3DScene() const;
    static void createObjects();
    void setCamera(const std::shared_ptr<Camera> & cam) {
        m_Camera = cam;
    }

    void setShader(Shader* shader) {
        shaderProgram = shader;
    }
    void DrawGrid(float gridSize, float gridStep) const;

private:
    Shader* shaderProgram;
    Shader* wireframe;
    Transform gridTransform;

    std::shared_ptr<Camera> m_Camera;

};

#endif //LUPUSFIRE_CORE_SCENE_H
