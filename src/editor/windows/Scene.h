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
#include "../../components/Gizmo.h"

class Scene {
public:
    Scene();
    ~Scene();

    void Render3DScene(float display_w, float display_h);
    void renderSceneView(int display_w, int display_h);
    void setShader(Shader& shader);
    void setCamera(Camera& camera);

    void addGameObject(std::shared_ptr<GameObject> object) {
        std::cout << "Adding GameObject: " << object->getName() << std::endl;
        m_objects.push_back(std::move(object));
    }

    const std::vector<std::shared_ptr<GameObject>> &getGameObjects() const {
        return m_objects;
    }

    void selectObject(std::shared_ptr<GameObject> object) {
        if (m_selectedObject) {
            m_selectedObject->deselect();
        }
        m_selectedObject = object;
        if (m_selectedObject) {
            m_selectedObject->select();
        }
    }

    void deselectCurrentObject() {
        if (m_selectedObject) {
            m_selectedObject->deselect();
            m_selectedObject.reset();
        }
    }

    void printMatrix(const glm::mat4& matrix);
private:
    InspectorManager* inspectorManager;
    std::vector<std::shared_ptr<GameObject>> m_objects;
    Shader* m_shader;
    Camera* m_camera;
    std::shared_ptr<GameObject> m_selectedObject;

    int FBO_width;
    int FBO_height;
    GLuint FBO;
    GLuint RBO;
    GLuint VAO;
    GLuint VBO;
    std::shared_ptr<Gizmo> m_gizmo;

    GLuint texture_id;
    bool depthTestEnabled = false;

    Transform gridTransform;

    float fov;
    void create_framebuffer(float display_w, float display_h);
    void DrawGrid(float gridSize, float gridStep);

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};

#endif //LUPUSFIRE_CORE_SCENE_H
