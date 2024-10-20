//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_RENDERER_H
#define LUPUSFIRE_CORE_RENDERER_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "AssetManager.h"
#include "HierarchyManager.h"
#include "ImGuizmo.h"
#include "InspectorManager.h"
#include "ProjectExplorer.h"
#include "Scene.h"
#include ".h/Ray.h"

class Renderer {
public:
    Renderer(Scene* scene, const std::shared_ptr<Camera>& camera, const std::shared_ptr<LightManager> & lightManager, GLFWwindow* window);
    ~Renderer();

    void render();

    void renderSceneView();
    void createSceneFramebuffer(int sceneWidth, int sceneHeight);

    void renderSceneViewport(int viewportWidth, int viewportHeight, GLuint framebuffer);
    void renderImGuizmo() const;
    GLuint getFramebufferID() const { return FBO; }
    GLuint getTextureID() const { return texture_id; }
    ImVec2 sceneViewportPos;
    ImVec2 sceneViewportSize;

    const std::shared_ptr<Camera> & getCamera() {
        return m_Camera;
    }

    void renderToolbar();
    void renderImGuiLayout();
    void SetupDockingLayout(ImGuiID dockspace_id);
    void renderHierarchy();
    void renderInspector();
    void renderProjectExplorer();
    void mouseMovementCallback(float xOffset, float yOffset, bool constrainPitch) const;
    void scrollCallback(float yoffset) const;
    void processInput(GLFWwindow* window);
    void ShowMemoryUsageWindow();
    void RenderContextMenu() const;
    GLuint loadTexture(const char* path);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void setMouseButtonCallback();
    void setScrollCallback();
    void setCursorPosCallback();
    void setKeyCallback();

private:
    AssetManager& assetManager = AssetManager::getInstance();
    std::shared_ptr<AssetItem> rootFolder = assetManager.getRootFolder();
    Scene* scene;
    bool dockspace_initialized = false;
    bool noLightSelectedLogged = false;

    const float selectionRadius = 0.5f;
    Renderer* myRenderer;
    std::shared_ptr<LightManager> lightManager;
    GLFWwindow* m_Window;
    const std::shared_ptr<Camera> & m_Camera;

    bool m_rightMouseButtonPressed = false;
    double m_lastX = 0.0f, m_lastY = 0.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float m_initialYaw, m_initialPitch;

    // Mouse input handling
    float lastX = 400, lastY = 300;

    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;

    float FBO_width;
    int FBO_height;
    GLuint FBO;
    GLuint RBO;
    GLuint texture_id;

    ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE currentGizmoMode = ImGuizmo::WORLD;
};


#endif //NAV2SFM Core_RENDERER_H
