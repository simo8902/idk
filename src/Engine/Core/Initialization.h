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
    static void initializeImGui(GLFWwindow *window);

    Shader* getShader() const;
    Scene* getScene() const;

    Shader* getWireframeShader() const;
    GLFWwindow* getWindow() const;
    const std::shared_ptr<Camera> & getMainCamera();

    void cameraInit();
private:
    Scene* scene;
    Shader* shaderProgram;
    Shader* wireframe;
    GLFWwindow* m_Window;
    std::shared_ptr<Camera> m_MainCamera;
    std::shared_ptr<Camera> m_SecondCamera;
    std::shared_ptr<Camera> m_ActiveCamera;

    std::shared_ptr<Renderer> m_Renderer;

    Initialization(Initialization const&) = delete;
    void operator=(Initialization const&) = delete;

protected:
    void switchToMainCamera() {
        m_ActiveCamera = m_MainCamera;
    }

    void switchToSecondCamera() {
        m_ActiveCamera = m_SecondCamera;
    }

};

#endif //LUPUSFIRE_CORE_INITIALIZATION_H
