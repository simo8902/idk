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
    Renderer(int width, int height, const char *title, float fov, float aspectRatio, float nearPlane, float farPlane);
    ~Renderer();

    void render();

    GLFWwindow * createGLFWWindow(int width, int height, const char* title);
    bool initializeGLFW();
    bool initializeOpenGL();
    void initializeImGui(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    bool ShouldClose();
    Ray generateRayFromMouse(const glm::vec2& ndc, int display_w, int display_h);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
   // void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);

private:
    GLFWwindow* m_Window;
    int m_WindowWidth;
    int m_WindowHeight;
    Camera m_camera;
    Scene* globalScene;
    SceneLoader* loader;
    std::shared_ptr<Shader> shaderProgram;
    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;
};


#endif //LUPUSFIRE_CORE_RENDERER_H
