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
#include "../../components/Transform.h"
#include "../../components/colliders/Ray.h"

class HierarchyManager;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initialization() {
        glm::vec3 position = glm::vec3(0.0f, 3.0f, -7.0f);
        glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        float yaw = -90.0f;
        float pitch = 0.0f;
        float moveSpeed = 2.0f;
        float mouseSensitivity = 2.0f;
        float fov = 70.0f;
        float nearPlane = 0.01f;
        float farPlane = 1000.0f;
        const std::string name = "Main Camera";
        m_Camera = std::make_shared<Camera>(name, position, forward, up, yaw, pitch, moveSpeed, mouseSensitivity, fov, nearPlane, farPlane);
        m_Camera->printCameraParams();

        m_Camera->processKeyboard(CameraMovement::FORWARD, 0.1f);

        //TODO:
        m_Camera->processMouseMovement(0.5f, 0.3f);

    }

    void render();

    GLFWwindow * createGLFWWindow(int width, int height);
    bool initializeGLFW();
    bool initializeOpenGL();
    void initializeImGui(GLFWwindow* window);
    bool ShouldClose();

    /*
    void addGameObject(std::shared_ptr<GameObject> object) {
        std::cout << "Adding GameObject: " << object->getName() << std::endl;
        m_objects.push_back(std::move(object));
    }*/

    const std::vector<std::shared_ptr<GameObject>> &getGameObjects() const {
        return m_objects;
    }

    void renderSceneView();
    void createSceneFramebuffer(int sceneWidth, int sceneHeight);
    void initialize();
    void Render3DScene();
    void DrawGrid(float gridSize, float gridStep);
    Ray getRayFromScreenPoint(glm::vec2 ndc);
    void renderSceneViewport(int viewportWidth, int viewportHeight, GLuint framebuffer);
    void renderImGuizmo();

    std::shared_ptr<Camera> getCamera() const {
        return m_Camera;
    }

    void renderImGuiLayout();
    std::shared_ptr<GameObject> selectedObject = nullptr;

private:
    std::vector<std::shared_ptr<GameObject>> m_objects;
    GameObject* objects;

    Renderer* myRenderer;
    Transform gridTransform;
    GLFWwindow* m_Window;

    std::shared_ptr<Camera> m_Camera;
    Scene* globalScene;

    Shader* shaderProgram;
    Shader* wireframe;

    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;

    int FBO_width;
    int FBO_height;
    GLuint FBO;
    GLuint RBO;
    GLuint texture_id;
};


#endif //LUPUSFIRE_CORE_RENDERER_H
