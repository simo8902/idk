//
// Created by SIMEON on 4/5/2025.
//

#ifndef ENGINEACCESS_H
#define ENGINEACCESS_H

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "ECScheduler.h"
#include "Scene.h"

namespace IDK {
    class Scene;
    class ECScheduler;
}

struct EngineAccess {
    GLFWwindow* window;
    std::shared_ptr<IDK::Scene> scene;
    IDK::ECScheduler& scheduler;
};

#endif //ENGINEACCESS_H
