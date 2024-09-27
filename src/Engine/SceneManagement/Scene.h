//
// Created by Simeon on 4/5/2024.
//

#ifndef NAV2SFM_Core_SCENE_H
#define NAV2SFM_Core_SCENE_H

#include "Camera.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"
#include "../../Engine/Lighting/DirectionalLight.h"
#include ".h/Sphere.h"

class Scene {
public:
    Scene(Shader* shaderProgram, Shader* wireframe, Shader* sky, const std::shared_ptr<Camera> & camera, GLuint skyboxTexture);
    ~Scene();

    static std::vector<std::shared_ptr<GameObject>> objects;
    static std::vector<std::shared_ptr<Light>> lights;

    void Render3DScene() const;
    void createObjects();

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

    GLuint skyVAO, skyVBO;

    void setupSky();
    void renderSky() const;
private:

    GLuint skyboxTextureID;
    GLuint skyboxVAO, skyboxVBO;
    GLuint skyboxTexture;

    Shader* shaderProgram;
    Shader* wireframe;
    Shader* skyShader;

    Transform gridTransform;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;

    std::shared_ptr<Camera> m_Camera;

};

#endif //NAV2SFM_Core_SCENE_H
