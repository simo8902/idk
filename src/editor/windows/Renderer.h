//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_RENDERER_H
#define LUPUSFIRE_CORE_RENDERER_H

#include "Scene.h"
#include "../../Shader.h"

#include "GLFW/glfw3.h"
#include "HierarchyManager.h"
#include "InspectorManager.h"
#include "ProjectExplorer.h"
#include "SceneLoader.h"


class Renderer {
public:
    Renderer(int width, int height, const char* title);
    ~Renderer();

    void render();

    GLFWwindow * createGLFWWindow(int width, int height, const char* title);
    bool initializeGLFW();
    bool initializeOpenGL();
    void initializeImGui(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    bool ShouldClose();
private:
    int m_WindowWidth;
    int m_WindowHeight;

    GLFWwindow* m_Window;
    Scene* globalScene;
    Shader* shaderProgram;
    SceneLoader* loader;
    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;
};


#endif //LUPUSFIRE_CORE_RENDERER_H
