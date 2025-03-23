#ifndef LUPUSFIRE_CORE_CAMERA_H
#define LUPUSFIRE_CORE_CAMERA_H

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "glad/glad.h"
#include <string>
#include <iostream>
#include <utility>

#include "Profiler.h"

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};


class Camera {
public:
    Camera(std::string  name, glm::vec3 position, glm::vec3 forward, glm::vec3 up,
           float yaw, float pitch, float moveSpeed,
           float mouseSensitivity, float fov, float nearPlane, float farPlane)
        : m_position(position),
          m_forwardVec(forward),
          m_upVec(up),
          name(std::move(name)),
          m_yaw(yaw),
          m_pitch(pitch),
          m_movementSpeed(moveSpeed),
          m_mouseSensitivity(mouseSensitivity),
          m_fov(fov),
          m_nearPlane(nearPlane),
          m_farPlane(farPlane)
    {
        std::cerr << "CAMERA()" << std::endl;

        m_yaw = glm::degrees(atan2(m_forwardVec.z, m_forwardVec.x));
        m_pitch = glm::degrees(asin(m_forwardVec.y));

        updateProjectionMatrix();
        updateVectors();
        updateViewMatrix();

    }

    void updateVectors() {
        glm::vec3 forward;
        forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        forward.y = sin(glm::radians(m_pitch));
        forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_forwardVec = glm::normalize(forward);

        right = glm::normalize(glm::cross(m_forwardVec, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_upVec = glm::normalize(glm::cross(right, m_forwardVec));
    }
    void updateViewMatrix() {
        glm::vec3 target = m_position + m_forwardVec;
        m_viewMatrix = glm::lookAt(m_position, target, m_upVec);
    }

    void updateProjectionMatrix() {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), 16.0f / 9.0f, m_nearPlane, m_farPlane);
    }

    glm::mat4 getViewMatrix() {
        return m_viewMatrix;
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
    void setCamera(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up) {
        m_position = position;
        m_forwardVec = glm::normalize(forward);
        m_upVec = glm::normalize(up);

        // Recalculate yaw and pitch based on the forward vector
        m_yaw = glm::degrees(atan2(m_forwardVec.z, m_forwardVec.x));
        m_pitch = glm::degrees(asin(m_forwardVec.y));

        // Update matrices
        updateVectors();
        updateViewMatrix();
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

        if (direction == CameraMovement::UP)
            m_position.y += velocity;
        if (direction == CameraMovement::DOWN)
            m_position.y -= velocity;

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

    float getFOV() const {
        return m_fov;
    }

    void setFOV(float fov) {
        m_fov = fov;
        updateProjectionMatrix();
    }

    // Getter and Setter for Near Plane
    float getNearPlane() const {
        return m_nearPlane;
    }

    void setNearPlane(float nearPlane) {
        m_nearPlane = nearPlane;
        updateProjectionMatrix();
    }

    // Getter and Setter for Far Plane
    float getFarPlane() const {
        return m_farPlane;
    }

    void setFarPlane(float farPlane) {
        m_farPlane = farPlane;
        updateProjectionMatrix();
    }


    void setName(const std::string& newName) {
        name = newName;
    }


    void setPosition(const glm::vec3& position) {
        m_position = position;
        updateViewMatrix();
    }


    // Getter for Forward Vector
    glm::vec3 getForwardVector() const {
        return m_forwardVec;
    }

    // Getter and Setter for Up Vector
    glm::vec3 getUpVector() const {
        return m_upVec;
    }

    void setUpVector(const glm::vec3& upVec) {
        m_upVec = upVec;
        updateViewMatrix();
    }

    glm::vec3 m_position;
    glm::vec3 m_forwardVec;
    glm::vec3 m_upVec;


private:
    std::string name;
    glm::mat4 m_ProjectionMatrix{};
    glm::mat4 m_viewMatrix{};
    glm::vec3 right{};
    float m_yaw;
    float m_pitch;
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;
    float m_nearPlane;
    float m_farPlane;

};

#endif