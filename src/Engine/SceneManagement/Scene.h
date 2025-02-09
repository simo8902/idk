//
// Created by Simeon on 4/5/2024.
//

#ifndef Core_SCENE_H
#define Core_SCENE_H

#include "LightManager.h"
#include "Renderer.h"
#include "GameObject.h"

class Scene {
public:
    Scene(const std::shared_ptr<Shader> & shaderProgram, const std::shared_ptr<Shader> & lightingShader, const std::shared_ptr<Shader> & finalPassShader,
        const std::shared_ptr<Camera> & camera, const std::shared_ptr<LightManager> & lightManager);
    ~Scene();

    static std::vector<std::shared_ptr<GameObject>> objects;
    static std::vector<std::shared_ptr<Component>> components;

    static std::vector<std::shared_ptr<Light>> lights;
    static void createTemporalObject();

    static void createObjects();

    void setCamera(const std::shared_ptr<Camera> & cam) {
        m_Camera = cam;
    }

    void setShader(std::shared_ptr<Shader> shader) {
        shaderProgram = shader;
    }

    static void checkFramebufferStatus();
    void DrawGrid(float gridSize, float gridStep) const;

    void setupSky();
    void renderSky() const;
    void setRenderer(const std::shared_ptr<Renderer> &renderer);

    void updateViewportFramebuffer(int viewportWidth, int viewportHeight);

    void RenderGeometryPass() const;
    void RenderLightingPass() const;

    GLuint getLightingTexture() const;
    GLuint gPosition;
    GLuint gAlbedoSpec;
    GLuint gNormal;
    GLuint gBuffer;

    std::shared_ptr<Shader> lightingShader;
    void initFullscreenQuad();
    void RenderQuad() const;
    GLuint lightingTexture;
    GLuint finalPassTexture;
    void RenderFinalPass() const;
    GLuint finalFramebuffer;

    void initFinalFramebuffer();

private:
    std::vector<Vertex> gridVertices;

    GLuint lightingFramebuffer;
    GLuint rboDepth;
    int width, height;
    GLuint quadVAO{};
    GLuint quadVBO{};

    GLuint skyboxTexture;
    GLuint skyVAO, skyVBO;
    mutable GLuint gridVAO;
    mutable GLuint gridVBO;

    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Shader> shaderProgram;
    std::shared_ptr<Shader> skyShader;
    std::shared_ptr<Shader> finalPassShader;

    Transform gridTransform;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;

    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<LightManager> lightManager;
};

#endif