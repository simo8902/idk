//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_RENDERER_H
#define LUPUSFIRE_CORE_RENDERER_H

#include "ImGuizmo.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "../../Shader.h"
#include "../../Camera.h"
#include "../../GameObject.h"
#include "../../components/Transform.h"
#include "../../components/colliders/Ray.h"
#include "../../Cube.h"
#include "../../components/colliders/BoxCollider.h"

#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "InspectorManager.h"
#include "ProjectExplorer.h"
#include "HierarchyManager.h"
#include "../../SubGameObject.h"

class Renderer {
public:
    Renderer(Shader* shaderProgram, Shader* wireframe, std::shared_ptr<Camera> camera, GLFWwindow* m_Window);
    ~Renderer();

    void render();


    void renderSceneView();
    void createSceneFramebuffer(int sceneWidth, int sceneHeight);
    void createObjects();
    void Render3DScene();
    void DrawGrid(float gridSize, float gridStep);

    void renderSceneViewport(int viewportWidth, int viewportHeight, GLuint framebuffer);
    void renderImGuizmo();

    std::shared_ptr<Camera> getCamera() const {
        return m_Camera;
    }

    void renderToolbar();
    void renderImGuiLayout();
    std::shared_ptr<SubGameObject> objects = nullptr;
    std::shared_ptr<GameObject> selectedObjects = nullptr;
    std::shared_ptr<SubGameObject> copiedObject = nullptr;

private:
    std::shared_ptr<SubGameObject> selectedSubObject = nullptr;
    Shader* shaderProgram;
    Shader* wireframe;
    Renderer* myRenderer;
    Transform gridTransform;
    GLFWwindow* m_Window;
    std::shared_ptr<Camera> m_Camera;

    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;

    int FBO_width;
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


#endif //LUPUSFIRE_CORE_RENDERER_H
