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
#include "GLFW/glfw3.h"

class Scene {
public:
    Scene();
    ~Scene();

    void Render3DScene(float display_w, float display_h);
    void setShader(Shader& shader);
    void setCamera(Camera& camera);



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

    static Scene* getCurrentScene(){
        return globalScene;
    }
    void setScene(Scene& scene);
    GLFWwindow* setWindow(GLFWwindow* window){
        return m_window;
    }


private:
    static Scene* globalScene;
    GLFWwindow* m_window;

    InspectorManager* inspectorManager;
    Shader* m_shader;
    Camera* m_camera;
    std::shared_ptr<GameObject> m_selectedObject;

    float fov;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};

#endif //LUPUSFIRE_CORE_SCENE_H
