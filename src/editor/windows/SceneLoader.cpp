//
// Created by Simeon on 4/15/2024.
//

#include "SceneLoader.h"
#include "../../Cube.h"
#include "../../components/colliders/BoxCollider.h"
#include "gtx/string_cast.hpp"
#include "Renderer.h"

SceneLoader::SceneLoader(){
    
}

void SceneLoader::init() {
    //CUBE1
    if (obj == nullptr){
        std::cerr << "null";
    }
    std::shared_ptr<Cube> cube1 = obj->addObject<Cube>("Cube1");
    cube1->addComponent<Transform>();
    cube1->setColor(glm::vec3(0.2f, 0.2f, 0.2f));

    Transform* cube1Transform = cube1->getComponent<Transform>();
    std::cerr << glm::to_string(cube1Transform->getPosition()) << std::endl;

    cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-3.0f), glm::vec3(3.0f));
    cube1Transform->setPosition(glm::vec3(1.0f, 1.5f, -2.5f));


    /*
    //CUBE2
    std::shared_ptr<Cube> cube2 = obj->addObject<Cube>("Cube2");
    cube2->addComponent<Transform>();
    //   cube2->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

    Transform* cube2Transform = cube2->getComponent<Transform>();

    cube2->addComponent<BoxCollider>(cube2Transform->getPosition(), glm::vec3(-1.0f), glm::vec3(1.0f));
    cube2Transform->setPosition(glm::vec3(-1.0f, 1.5f, -2.5f));
    */
    //globalScene->addGameObject(cube2);
}