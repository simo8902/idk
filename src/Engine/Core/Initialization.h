//
// Created by Simeon on 4/27/2024.
//

#ifndef CORE_INITIALIZATION_H
#define CORE_INITIALIZATION_H

#include <thread>

#include "Shader.h"
#include "GLFW/glfw3.h"
#include "Renderer.h"
#include "Scene.h"

class Initialization{
public:
    explicit Initialization();
    ~Initialization();

    void runMainLoop() const;
    bool ShouldClose() const;

    void initializeImGui(GLFWwindow *window);

    Shader* getShader() const;
    const std::shared_ptr<Scene> & getScene() const;

    Shader* getWireframeShader() const;
    GLFWwindow* getWindow() const;
    std::shared_ptr<LightManager> getLightManager() const;

    void cameraInit();

    Initialization(Initialization const&) = delete;
    void operator=(Initialization const&) = delete;

    static void initImGuiStyle();
    void init();
    GLFWwindow* createWindow();


    bool isMainThread() {
        return std::this_thread::get_id() == mainThreadId;
    }
private:
    std::thread::id mainThreadId;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<LightManager> lightManager;

    GLFWwindow* m_Window;
    std::shared_ptr<Camera> m_MainCamera;
    std::shared_ptr<Renderer> m_Renderer;
    std::shared_ptr<Shader> shaderProgram;
    std::shared_ptr<Shader> lightShader;
    std::shared_ptr<Shader> finalPassShader;

    std::vector<std::shared_ptr<AssetItem>> assets;
};

#endif