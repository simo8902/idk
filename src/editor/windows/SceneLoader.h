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
        std::shared_ptr<Cube> cube1 = obj->addObject<Cube>("Damn Cube");
        cube1->addComponent<Transform>();
        cube1->addComponent<BoundingBox>();
        cube1->setColor(glm::vec3(1.0f, 1.0f, 0.0f));


        Transform* cube1Transform = cube1->getComponent<Transform>();
        cube1Transform->setRotation(glm::vec3(0.0f, 0.0f, 45.0f));
        cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-1.0f), glm::vec3(1.0f));

        //CUBE2
        std::shared_ptr<Cube> cube2 = obj->addObject<Cube>("Damn 2 Cube");
        cube2->addComponent<Transform>();
        cube2->addComponent<BoundingBox>();
      //  cube2->setColor(glm::vec3(1.0f, 0.0f, 0.0f));

        Transform* cube2Transform = cube2->getComponent<Transform>();
        cube2Transform->setPosition(glm::vec3(-3.0f, 2.0f, 0.0f));

        //CUBE3
        std::shared_ptr<Cube> cube3 = obj->addObject<Cube>("Damn 3 Cube");
        cube3->addComponent<Transform>();
        cube3->addComponent<BoundingBox>();
      //  cube3->setColor(glm::vec3(0.0f, 0.0f, 1.0f));

        Transform* cube3Transform = cube3->getComponent<Transform>();
        cube3Transform->setPosition(glm::vec3(3.0f, 2.0f, 0.0f));

        globalScene->addGameObject(cube1);
        globalScene->addGameObject(cube2);
        globalScene->addGameObject(cube3);
    }
private:
    GameObject* obj;
    std::shared_ptr<Scene> globalScene;
};


#endif //LUPUSFIRE_CORE_SCENELOADER_H
