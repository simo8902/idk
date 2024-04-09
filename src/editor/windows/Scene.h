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

    const std::vector<std::shared_ptr<GameObject>>& getGameObjects() const {
        return m_objects;
    }

    glm::mat4 calculateViewMatrix();
    glm::mat4 calculateProjectionMatrix(int display_w, int display_h);
    void renderGameView();
private:
    std::vector<std::shared_ptr<GameObject>> m_objects;

    std::shared_ptr<Shader> m_shader;

    int FBO_width;
    int FBO_height;
    GLuint FBO;
    GLuint RBO;
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
