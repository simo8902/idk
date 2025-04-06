//
// Created by SIMEON on 2/22/2025.
//

#ifndef FORWARDRENDERER_H
#define FORWARDRENDERER_H

#include "Scene.h"
#include "ShaderManager.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "IRenderForward.h"

class ForwardRenderer : public IRenderForward {
    std::shared_ptr<IDK::Scene> scene;
    std::shared_ptr<IDK::Graphics::Camera> camera;
    GLFWwindow* window;
    std::string rendererType;

    GLuint FBO;
    GLuint texture_id;
    GLuint RBO;
    int width;
    int height;
    std::shared_ptr<IDK::Graphics::Shader> shaderProgram = ShaderManager::Instance().getShaderProgram();

public:
    ForwardRenderer(const std::shared_ptr<IDK::Scene>& scene, const std::shared_ptr<IDK::Graphics::Camera>& camera,
                    GLFWwindow* window, const std::string& rendererType);
    ~ForwardRenderer() override;

    void render() override;
    void resizeFramebuffer(int width, int height) override;
    GLuint getFramebuffer() const override { return FBO; }
    GLuint getTexture() const override { return texture_id; }
    int getFBOWidth() const override { return width; }
    int getFBOHeight() const override { return height; }

    void updateFramebuffer(int viewportWidth, int viewportHeight) override;


};

#endif //FORWARDRENDERER_H
