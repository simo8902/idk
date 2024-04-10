//
// Created by Simeon on 4/5/2024.
//

#ifndef LUPUSFIRE_CORE_SCENE_H
#define LUPUSFIRE_CORE_SCENE_H

#include <vector>
#include <memory>
#include "../../GameObject.h"
#include "../../Shader.h"
#include "../../Camera.h"
#include "../../Cube.h"
#include "../../components/Transform.h"
#include "imgui.h"
#include "InspectorManager.h"
#include "../../components/colliders/BoxCollider.h"
#include "../../../Gizmo.h"

class Scene {
public:
    Scene(Shader *shader);
    ~Scene();

    void Render3DScene(float display_w, float display_h);
    void renderSceneView(int display_w, int display_h);
    void update();

    void addGameObject(std::shared_ptr<GameObject> object) {
        std::cout << "Adding GameObject: " << object->getName() << std::endl;
        m_objects.push_back(std::move(object));
    }

    const std::vector<std::shared_ptr<GameObject>> &getGameObjects() const {
        return m_objects;
    }

    glm::mat4 calculateViewMatrix();

    glm::mat4 calculateProjectionMatrix(int display_w, int display_h);

    void renderGameView();

    std::shared_ptr<GameObject> raycastFromMouse(const ImVec2 &mousePos, int display_w, int display_h);
    glm::vec2 calculateNormalizedPosition(const ImVec2& mousePos, int display_w, int display_h);
    void renderGizmo(Transform* transform);
    void renderRay(const Ray& ray, const glm::vec3& color);
    void printMatrix(const glm::mat4& matrix);
    bool aabbIntersection(const Ray& ray, const BoxCollider* boxCollider);

private:
    InspectorManager* inspectorManager;
    std::vector<std::shared_ptr<GameObject>> m_objects;
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<GameObject> m_selectedObject;

    int FBO_width;
    int FBO_height;
    GLuint FBO;
    GLuint RBO;
    GLuint VAO;
    GLuint VBO;
    std::shared_ptr<Gizmo> m_gizmo;

    GLuint texture_id;
    bool framebufferInitialized = false;
    bool depthTestEnabled = false;

    Transform gridTransform;

    float fov;
    void create_framebuffer(float display_w, float display_h);
    void DrawGrid(float gridSize, float gridStep);

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    Scene *scene;
    bool debugMode = true;
};

#endif //LUPUSFIRE_CORE_SCENE_H
