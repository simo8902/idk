//
// Created by Simeon on 4/4/2024.
//

#ifndef LUPUSFIRE_CORE_CAMERA_H
#define LUPUSFIRE_CORE_CAMERA_H

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Target;
    glm::vec3 Up;

    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
            : Position(position), Target(target), Up(up) {}

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Target, Up);
    }
};

#endif //LUPUSFIRE_CORE_CAMERA_H
