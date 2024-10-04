//
// Created by Simeon on 4/5/2024.
//

#ifndef NAV2SFM_Core_SCENE_H
#define NAV2SFM_Core_SCENE_H

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "../../Engine/Lighting/DirectionalLight.h"
#include ".h/Sphere.h"

class Scene {
public:
    Scene(const std::shared_ptr<Shader> & shaderProgram,const std::shared_ptr<Shader> & wireframe, const std::shared_ptr<Shader> & sky,
        const std::shared_ptr<Camera> & camera,const GLuint & skyboxTexture);
    ~Scene();

    static std::vector<std::shared_ptr<GameObject>> objects;
    static std::vector<std::shared_ptr<Light>> lights;

    void Render3DScene() const;
    void createObjects();

    void setCamera(const std::shared_ptr<Camera> & cam) {
        m_Camera = cam;
    }
    static void createTemporalObject();
    void setShader(std::shared_ptr<Shader> shader) {
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

    std::shared_ptr<Shader> shaderProgram;
    std::shared_ptr<Shader> wireframe;
    std::shared_ptr<Shader> skyShader;

    Transform gridTransform;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;

    std::shared_ptr<Camera> m_Camera;

};

#endif //NAV2SFM_Core_SCENE_H
