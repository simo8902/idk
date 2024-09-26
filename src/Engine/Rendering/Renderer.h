//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_RENDERER_H
#define LUPUSFIRE_CORE_RENDERER_H
#include "glad/glad.h"

#include <GLFW/glfw3.h>

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

    void scrollCallback(float yoffset) const;
    void mouseMovementCallback(float xOffset, float yOffset, bool constrainPitch) const;
    void setMainCamera(const std::shared_ptr<Camera> & camera);

    void render();
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void renderSceneView();
    void initImGuiStyle() const;
    void createSceneFramebuffer(int sceneWidth, int sceneHeight);

    void renderSceneViewport(int viewportWidth, int viewportHeight, GLuint framebuffer);
    void renderImGuizmo();
    void RenderContextMenu() const;
    GLuint getFramebufferID() const { return FBO; }
    GLuint getTextureID() const { return texture_id; }


   std::shared_ptr<Camera>  getCamera() const {
        return m_Camera;
    }
    void selectLight(const std::shared_ptr<Light>& light);
    bool intersectsWithLight(const Ray& ray, const Light& light, float& distance, float selectionRadius);

    std::shared_ptr<LightManager>  getLight() const {
        return lightManager;
    }
    void selectObject(const std::shared_ptr<GameObject>& object) {
        selectedObjects = object;
    }
    std::shared_ptr<Light> selectedLight;
    static void ShowMemoryUsageWindow();

    std::shared_ptr<GameObject> selectedObjects = nullptr;
    std::shared_ptr<GameObject> clipboardObject = nullptr;
    void processInput(GLFWwindow* window);
    void renderToolbar();
    void renderImGuiLayout();
    Scene* scene;

private:
    bool noLightSelectedLogged = false;
    bool noObjectSelectedLogged = false;

    const float selectionRadius = 0.5f;
    Renderer* myRenderer;
    std::shared_ptr<LightManager> lightManager;
    GLFWwindow* m_Window;
    std::shared_ptr<Camera> m_Camera;

    bool shiftPressed = false;
    bool m_rightMouseButtonPressed = false;
    double m_lastX = 0.0f, m_lastY = 0.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float m_initialYaw, m_initialPitch;
    bool m_initialOrientationSet = false;

    // Mouse input handling
    float lastX = 400, lastY = 300;
    bool firstMouse = true;


    bool isPosButtonActive = false;




    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;

    float FBO_width;
    int FBO_height;
    GLuint FBO;
    GLuint RBO;
    GLuint texture_id;

    enum GizmoMode {
        TRANSLATE,
        ROTATE,
        SCALE
    };

    ImGuizmo::OPERATION currentGizmoMode = ImGuizmo::TRANSLATE;

};


#endif //NAV2SFM Core_RENDERER_H
