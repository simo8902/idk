//
// Created by Simeon on 4/27/2024.
//

#ifndef LUPUSFIRE_CORE_INITIALIZATION_H
#define LUPUSFIRE_CORE_INITIALIZATION_H

#include "Shader.h"
#include "GLFW/glfw3.h"
#include "Renderer.h"
#include "Scene.h"

class Initialization{
public:
    Initialization();
    ~Initialization();

    void runMainLoop() const;
    bool ShouldClose() const;

    static bool initializeGLFW();
    static bool initializeOpenGL();
    static GLFWwindow* createGLFWWindow(int width, int height);
    static void errorCallback(int error, const char *description) ;
    void initializeImGui(GLFWwindow *window);

    Shader* getShader() const;
    Scene* getScene() const;

    Shader* getWireframeShader() const;
    GLFWwindow* getWindow() const;
    const std::shared_ptr<Camera> & getMainCamera();
    std::shared_ptr<LightManager> getLightManager() const;

    void cameraInit();

    Initialization(Initialization const&) = delete;
    void operator=(Initialization const&) = delete;
    GLuint loadCubemap(std::vector<std::string> faces);
    void initImGuiStyle();
private:
    Scene* scene;
    Shader* shaderProgram;
    Shader* wireframe;
    Shader* skyShaderProgram;
    GLuint skyboxTexture;

    std::shared_ptr<LightManager> lightManager;

    GLFWwindow* m_Window;
    std::shared_ptr<Camera> m_MainCamera;
    std::shared_ptr<Camera> m_SecondCamera;
    std::shared_ptr<Camera> m_ActiveCamera;

    std::shared_ptr<Renderer> m_Renderer;

protected:
    void switchToMainCamera() {
        m_ActiveCamera = m_MainCamera;
    }

    void switchToSecondCamera() {
        m_ActiveCamera = m_SecondCamera;
    }

};

#endif //NAV2SFM Core_INIT_H
