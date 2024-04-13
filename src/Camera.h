//
// Created by Simeon on 4/4/2024.
//

#ifndef LUPUSFIRE_CORE_CAMERA_H
#define LUPUSFIRE_CORE_CAMERA_H

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 upDirection, float fov, float aspectRatio, float nearPlane, float farPlane)
            : m_position(position), m_target(target), m_upDirection(upDirection), m_fov(fov), m_aspectRatio(aspectRatio), m_nearPlane(nearPlane), m_farPlane(farPlane) {}

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(m_position, m_target, m_upDirection);
    }

    glm::mat4 getProjectionMatrix(float display_w, float display_h) const {
        return glm::perspective(glm::radians(m_fov), display_w / display_h, m_nearPlane, m_farPlane);
    }

    glm::vec3 getPosition() const {
        return m_position;
    }

    glm::vec3 getDirection() const {
        return m_target;
    }

    void printCameraParams() const {
        std::cout << "Position: (" << m_position.x << ", " << m_position.y << ", " << m_position.z << ")" << std::endl;
        std::cout << "Target: (" << m_target.x << ", " << m_target.y << ", " << m_target.z << ")" << std::endl;
        std::cout << "Up Direction: (" << m_upDirection.x << ", " << m_upDirection.y << ", " << m_upDirection.z << ")" << std::endl;
        std::cout << "Field of View: " << m_fov << std::endl;
        std::cout << "Aspect Ratio: " << m_aspectRatio << std::endl;
        std::cout << "Near Plane: " << m_nearPlane << std::endl;
        std::cout << "Far Plane: " << m_farPlane << std::endl;
    }

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_upDirection;
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
};

#endif //LUPUSFIRE_CORE_CAMERA_H
