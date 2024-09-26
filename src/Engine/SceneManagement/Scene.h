//
// Created by Simeon on 4/5/2024.
//

#ifndef LUPUSFIRE_CORE_SCENE_H
#define LUPUSFIRE_CORE_SCENE_H

#include "Camera.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"
#include "../../Engine/Lighting/DirectionalLight.h"

class Scene {
public:
    Scene(Shader* shaderProgram, Shader* wireframe,Shader* lighting, const std::shared_ptr<Camera> & camera);
    ~Scene();


    static std::vector<std::shared_ptr<GameObject>> objects;
    static std::vector<std::shared_ptr<Light>> lights;

    void Render3DScene(const Renderer& renderer) const;
    static void createObjects();

    void setCamera(const std::shared_ptr<Camera> & cam) {
        m_Camera = cam;
    }
    static void createTemporalObject();
    void setShader(Shader* shader) {
        shaderProgram = shader;
    }
    void DrawGrid(float gridSize, float gridStep) const;

    void addDirectionalLight(const std::shared_ptr<DirectionalLight>& light) {
        directionalLights.push_back(light);
    }

private:
    Shader* shaderProgram;
    Shader* wireframe;
    Shader* lightingShader;

    Transform gridTransform;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;

    std::shared_ptr<Camera> m_Camera;

};

#endif //NAV2SFM Core_SCENE_H
