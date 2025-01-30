//
// Created by Simeon on 4/27/2024.
//

#ifndef CORE_INITIALIZATION_H
#define CORE_INITIALIZATION_H

#include "Shader.h"
#include "GLFW/glfw3.h"
#include "Renderer.h"
#include "Scene.h"

class Initialization{
public:
    explicit Initialization(GLFWwindow* window);
    ~Initialization();

    void runMainLoop() const;
    bool ShouldClose() const;

    static void initializeImGui(GLFWwindow *window);

    Shader* getShader() const;
    const std::shared_ptr<Scene> & getScene() const;

    Shader* getWireframeShader() const;
    GLFWwindow* getWindow() const;
    std::shared_ptr<LightManager> getLightManager() const;

    void cameraInit();

    Initialization(Initialization const&) = delete;
    void operator=(Initialization const&) = delete;
    static void initImGuiStyle();
private:
    std::shared_ptr<Scene> scene;
    std::shared_ptr<LightManager> lightManager;

    GLFWwindow* m_Window;
    std::shared_ptr<Camera> m_MainCamera;
    std::shared_ptr<Renderer> m_Renderer;
    std::vector<std::shared_ptr<AssetItem>> assets;
};

#endif