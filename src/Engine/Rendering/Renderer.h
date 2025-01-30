//
// Created by Simeon on 4/7/2024.
//

#ifndef CORE_RENDERER_H
#define CORE_RENDERER_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "InspectorManager.h"
#include "ProjectExplorer.h"
#include "FPSCounter.h"
#include "HierarchyManager.h"
#include "ImGuizmo.h"

class Renderer {
public:
    Renderer(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Camera>& camera, const std::shared_ptr<LightManager> & lightManager, GLFWwindow* window);
    ~Renderer();

    void render();

    void renderSceneView();
   // void createSceneFramebuffer(int width, int height);

    void renderImGuizmo() const;

    void onWindowResize(int width, int height);

    const std::shared_ptr<Camera> & getCamera() const {
        return m_Camera;
    }
    static void SetupDockingLayout(const ImGuiID & dockspace_id);

    void renderToolbar();
    void renderImGuiLayout();
    void renderHierarchy() const;
    void renderInspector();
    void renderProjectExplorer();
    void mouseMovementCallback(float xOffset, float yOffset, bool constrainPitch) const;
    void scrollCallback(float yoffset) const;
    void processInput(GLFWwindow* window);
    void RenderContextMenu() const;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void setMouseButtonCallback() const;
    void setScrollCallback() const;
    void setCursorPosCallback() const;
    void setKeyCallback() const;
    void framebuffer_size_callback(GLFWwindow* window, const int &width, const int &height);
    static void framebuffer_size_callback_static(GLFWwindow* window, const int width, const int height);

private:
    bool framebufferResized = false;
    int newWidth = 0;
    int newHeight = 0;

    bool dockspace_initialized = false;
    bool noLightSelectedLogged = false;

    const float selectionRadius = 0.5f;
    Renderer* myRenderer;
    const std::shared_ptr<LightManager>& lightManager;
    const std::shared_ptr<Scene>& scene;
    const std::shared_ptr<Camera> & m_Camera;

    GLFWwindow* m_Window;

    bool m_rightMouseButtonPressed = false;
    double m_lastX = 0.0f, m_lastY = 0.0f;
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    double m_initialYaw{}, m_initialPitch{};

    float lastX = 400, lastY = 300;

    FPSCounter fpsCounter;
    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;

    int FBO_width{};
    int FBO_height{};
    GLuint FBO{};
    GLuint RBO{};
    GLuint texture_id{};

    ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE currentGizmoMode = ImGuizmo::WORLD;
};

#endif