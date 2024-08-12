//
// Created by Simeon on 4/27/2024.
//

#ifndef LUPUSFIRE_CORE_INITIALIZATION_H
#define LUPUSFIRE_CORE_INITIALIZATION_H

#include "Shader.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "editor/windows/Scene.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "editor/windows/Renderer.h"

class Renderer;

class Initialization{
public:
    Initialization();
    ~Initialization();

    void runMainLoop();

    bool initializeGLFW();
    bool initializeOpenGL();
    bool ShouldClose();

    static void errorCallback(int error, const char *description) {
        std::cerr << "Error: " << error << " " << description << std::endl;
    }

    GLFWwindow* createGLFWWindow(int width, int height);

    void initializeImGui(GLFWwindow *window);

    Shader* getShaderProgram(){
        return shaderProgram;
    }

    Shader* getWireFrameProgram(){
        return wireframe;
    }


    std::shared_ptr<Camera> getMainCamera(){
        return m_Camera;
    }

    GLFWwindow* getWindow(){
        return m_Window;
    }

    void cameraInit();
private:
    Shader* shaderProgram;
    Shader* wireframe;
  //  Scene* globalScene;
    GLFWwindow* m_Window;
    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<Renderer> m_Renderer;

    Initialization(Initialization const&) = delete; // Copy constructor is deleted
    void operator=(Initialization const&) = delete; // Copy assignment is deleted

};





#endif //LUPUSFIRE_CORE_INITIALIZATION_H
