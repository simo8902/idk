//
// Created by SIMEON on 2/22/2025.
//

#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <memory>
#include <vector>

#include "Scene.h"
#include "Camera.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "ShaderManager.h"
#include "IRenderDeferred.h"

class Shader;

class DeferredRenderer final : public IRenderDeferred {
public:
    DeferredRenderer(const std::shared_ptr<IDK::Scene>& scene, const std::shared_ptr<IDK::Graphics::Camera>& camera,
                     GLFWwindow* window, const std::string& rendererType);
    ~DeferredRenderer() override;

    void resizeFramebuffer(int width, int height) override;
    GLuint getFramebuffer() const override { return gBuffer; }
    GLuint getTexture() const override { return lightingTexture; }
    int getFBOWidth() const override { return width; }
    int getFBOHeight() const override { return height; }
    void render() override;
    void updateViewportFramebuffer(const int & viewportWidth,const int & viewportHeight) override;

    void initFullscreenQuad();

    void RenderGeometryPass() const;
    void RenderLightingPass() const;
    void RenderFinalPass() const;

    void checkFramebufferStatus();
    void initLightManager();

    GLuint getGPosition() override {
        return gPosition;
    }

    GLuint getGNormal() override {
        return gNormal;
    }

    GLuint getGAlbedoSpec() override {
        return gAlbedoSpec;
    }
private:
    GLuint gPosition, gNormal, gAlbedoSpec;

    std::shared_ptr<IDK::Graphics::Shader> shaderProgram = ShaderManager::Instance().getShaderProgram();
    std::shared_ptr<IDK::Graphics::Shader> lightingShader = ShaderManager::Instance().getLightShader();
    std::shared_ptr<IDK::Graphics::Shader> finalPassShader = ShaderManager::Instance().getFinalPassShader();

    GLFWwindow* window;
    std::string rendererType;

    GLuint gBuffer;
    GLuint rboDepth;
    GLuint lightingFramebuffer;
    GLuint lightingTexture;
    GLuint finalFramebuffer;
    GLuint finalPassTexture;
    GLuint quadVAO, quadVBO;
    int width, height;

    std::shared_ptr<IDK::Scene> scene;
    std::shared_ptr<IDK::Graphics::Camera> camera;
    std::shared_ptr<LightManager> lightManager;
};


#endif //DEFERREDRENDERER_H
