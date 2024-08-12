//
// Created by Simeon on 4/4/2024.
//

#ifndef LUPUSFIRE_CORE_CAMERA_H
#define LUPUSFIRE_CORE_CAMERA_H

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include <string>
#include <iostream>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};


class Camera {
public:
    Camera(const std::string& name, glm::vec3 position, glm::vec3 forward, glm::vec3 up,
           float yaw, float pitch, float moveSpeed,
           float mouseSensitivity, float fov, float nearPlane, float farPlane)
        : m_position(position),
          m_forwardVec(forward),
          m_upVec(up),
          name(name),
          m_yaw(yaw),
          m_pitch(pitch),
          m_movementSpeed(moveSpeed),
          m_mouseSensitivity(mouseSensitivity),
          m_fov(fov),
          m_nearPlane(nearPlane),
          m_farPlane(farPlane)

    {
        updateProjectionMatrix();
        updateVectors();
        updateViewMatrix();

    }

    void updateViewMatrix() {
        glm::vec3 target = m_position + m_forwardVec;
        m_viewMatrix = glm::lookAt(m_position, target, m_upVec);
    }

    glm::mat4 getViewMatrix() {
        return m_viewMatrix;
    }

    void updateProjectionMatrix() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), 16.0f / 9.0f, m_nearPlane, m_farPlane);
    }

    void setProjectionMatrix(const glm::mat4& projection) {
        m_ProjectionMatrix = projection;
    }

    const glm::mat4& getProjectionMatrix() const {
        return m_ProjectionMatrix;
    }

    std::string getName() const {
        return name;
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(m_position, m_forwardVec, m_upVec);
    }

    void processKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = m_movementSpeed * deltaTime;
        if (direction == CameraMovement::FORWARD)
            m_position += m_forwardVec * velocity;
        if (direction == CameraMovement::BACKWARD)
            m_position -= m_forwardVec * velocity;
        if (direction == CameraMovement::LEFT)
            m_position -= glm::normalize(glm::cross(m_forwardVec, m_upVec)) * velocity;
        if (direction == CameraMovement::RIGHT)
            m_position += glm::normalize(glm::cross(m_forwardVec, m_upVec)) * velocity;

        updateViewMatrix();
    }

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= m_mouseSensitivity;
        yoffset *= m_mouseSensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (constrainPitch) {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        updateVectors();
        updateViewMatrix();
    }


    void processMouseScroll(float yoffset) {
        m_fov -= yoffset;
        // m_fov = std::clamp(m_fov, 0.1f, 89.90f);

        if (m_fov < 1.0f)
            m_fov = 1.0f;
        if (m_fov > 45.0f)
            m_fov = 45.0f;

        updateProjectionMatrix();
    }

    void processScroll(double yOffset) {
        float scrollSpeed = 1.5f;
        m_position += scrollSpeed * static_cast<float>(yOffset) * m_forwardVec;
        updateViewMatrix();
    }

    void printCameraParams() const {
        std::cout << "Position: (" << m_position.x << ", " << m_position.y << ", " << m_position.z << ")" << std::endl;
        std::cout << "Forward: (" << m_forwardVec.x << ", " << m_forwardVec.y << ", " << m_forwardVec.z << ")" << std::endl;
        std::cout << "Up: (" << m_upVec.x << ", " << m_upVec.y << ", " << m_upVec.z << ")" << std::endl;
        std::cout << "Fov: " << m_fov << std::endl;
        std::cout << "Near Plane: " << m_nearPlane << std::endl;
        std::cout << "Far Plane: " << m_farPlane << std::endl;
    }

    float getFov() const {
        return m_fov;
    }

    glm::vec3 getPosition() const {
        return m_position;
    }

    float getYaw() const {
        return m_yaw;
    }

    float getPitch() const {
        return m_pitch;
    }

    float setYaw(float newYaw) {
        return m_yaw = newYaw;
    }

    float setPitch(float newPitch) {
        return m_pitch = newPitch;
    }

    glm::vec3 m_position;
    glm::vec3 m_forwardVec;
    glm::vec3 m_upVec;

private:
    std::string name;
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::vec3 right;
    float m_yaw;
    float m_pitch;
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;
    float m_nearPlane;
    float m_farPlane;

    void updateVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_forwardVec = glm::normalize(front);

        right = glm::normalize(glm::cross(m_forwardVec, m_upVec));
    }
};

#endif //LUPUSFIRE_CORE_CAMERA_H
