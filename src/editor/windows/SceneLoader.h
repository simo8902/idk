//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_SCENELOADER_H
#define LUPUSFIRE_CORE_SCENELOADER_H

#include "../../GameObject.h"

class SceneLoader {
public:
    SceneLoader();

    void init();

private:
    GameObject* obj;
};


#endif //LUPUSFIRE_CORE_SCENELOADER_H
