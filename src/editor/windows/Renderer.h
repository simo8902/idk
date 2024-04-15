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

class HierarchyManager;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initialization(int width, int height, const char *title, float fov, float aspectRatio, float nearPlane,
                        float farPlane) {
        glm::vec3 position = glm::vec3(0.0f, 1.0f, -10.0f);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

        m_Camera = new Camera(position, target, upDirection, fov, aspectRatio, nearPlane, farPlane);

        this->m_width = width;
        this->m_height = height;
        this->m_title = title;
        this->m_fov = fov;
        this->m_aspectRatio = aspectRatio;
        this->m_nearPlane = nearPlane;
        this->m_farPlane = farPlane;
    }

    void render();

    GLFWwindow * createGLFWWindow(int width, int height);
    bool initializeGLFW();
    bool initializeOpenGL();
    void initializeImGui(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    bool ShouldClose();

    void addGameObject(std::shared_ptr<GameObject> object) {
        std::cout << "Adding GameObject: " << object->getName() << std::endl;
        m_objects.push_back(std::move(object));
    }

    const std::vector<std::shared_ptr<GameObject>> &getGameObjects() const {
        return m_objects;
    }

    void renderSceneView();
    void create_framebuffer();
    void initialize();
    void DrawGrid(float gridSize, float gridStep);
    void Render3DScene();
    Ray generateRayFromMouse(const glm::vec2& ndc);
private:
    std::vector<std::shared_ptr<GameObject>> m_objects;
    GameObject* object;
    Renderer* myRenderer;
    Transform gridTransform;
    GLFWwindow* m_Window;
    int display_w = 1280;
    int display_h = 720;

    Camera* m_Camera;
    Scene* globalScene;
    std::shared_ptr<Shader> shaderProgram;
    HierarchyManager hierarchyManager;
    InspectorManager inspectorManager;
    ProjectExplorer projectExplorer;

    int m_width;
    int m_height;
    const char *m_title;
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    int FBO_width;
    int FBO_height;
    GLuint FBO;
    GLuint RBO;
    GLuint VAO;
    GLuint VBO;
    GLuint texture_id;
    bool depthTestEnabled = false;


};


#endif //LUPUSFIRE_CORE_RENDERER_H
