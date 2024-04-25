//
// Created by Simeon on 4/5/2024.
//

#ifndef LUPUSFIRE_CORE_SCENE_H
#define LUPUSFIRE_CORE_SCENE_H

#include "../../Shader.h"
#include "../../Camera.h"

class Scene {
public:
    Scene();
    ~Scene();

    Camera* getCamera(){
        return m_camera;
    }

    void setShader(Shader& shader);
    void setCamera(Camera& camera);

    static Scene* getCurrentScene(){
        return globalScene;
    }

    void setScene(Scene& scene);

private:
    static Scene* globalScene;

    Shader* m_shader;
    Camera* m_camera;


};

#endif //LUPUSFIRE_CORE_SCENE_H
