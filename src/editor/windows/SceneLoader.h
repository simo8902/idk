//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_SCENELOADER_H
#define LUPUSFIRE_CORE_SCENELOADER_H

#include "../../GameObject.h"
#include "../../Cube.h"
#include "Scene.h"
#include "../../components/colliders/BoxCollider.h"

class SceneLoader {
public:
    SceneLoader(Scene* scene) : globalScene(scene){}

    void initialize(){
        //CUBE1
        std::shared_ptr<Cube> cube1 = obj->addObject<Cube>("Cube1");
        cube1->addComponent<Transform>();
     //   cube1->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

        Transform* cube1Transform = cube1->getComponent<Transform>();

        cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-1.0f), glm::vec3(1.0f));
        cube1Transform->setPosition(glm::vec3(1.0f, 1.5f, -2.5f));

        //CUBE2
        std::shared_ptr<Cube> cube2 = obj->addObject<Cube>("Cube2");
        cube2->addComponent<Transform>();
     //   cube2->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

        Transform* cube2Transform = cube2->getComponent<Transform>();

        cube2->addComponent<BoxCollider>(cube2Transform->getPosition(), glm::vec3(-1.0f), glm::vec3(1.0f));
        cube2Transform->setPosition(glm::vec3(-1.0f, 1.5f, -2.5f));

        globalScene->addGameObject(cube1);
        globalScene->addGameObject(cube2);
    }
private:
    GameObject* obj;
    std::shared_ptr<Scene> globalScene;
};


#endif //LUPUSFIRE_CORE_SCENELOADER_H
