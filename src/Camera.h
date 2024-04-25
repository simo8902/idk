//
// Created by Simeon on 4/4/2024.
//

#ifndef LUPUSFIRE_CORE_CAMERA_H
#define LUPUSFIRE_CORE_CAMERA_H

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"


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
           float mouseSensitivity, float fov, float nearPlane, float farPlane):
                name(name),
                m_position(position), m_forwardVec(forward), m_upVec(up), m_yaw(yaw), m_pitch(pitch),
                m_movementSpeed(moveSpeed), m_mouseSensitivity(mouseSensitivity),
                m_fov(fov), m_nearPlane(nearPlane), m_farPlane(farPlane)
    {
        updateProjectionMatrix();
        //  updateVectors();
    }

    void updateProjectionMatrix() {
        // You'll need to provide the aspect ratio (usually width / height of your viewport)
        m_ProjectionMatrix = glm::perspective(glm::radians(m_fov), 16.0f / 9.0f, m_nearPlane, m_farPlane);
    }
    void setProjectionMatrix(const glm::mat4& projection) {
        m_ProjectionMatrix = projection;
    }

    const glm::mat4& getProjectionMatrix() const {
        return m_ProjectionMatrix;
    }



    void updateVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        newFront.y = sin(glm::radians(m_pitch));
        newFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_forwardVec = glm::normalize(newFront);

        // Also re-calculate the Right and Up vector
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // normalized up vector, usually (0, 1, 0)
        glm::vec3 right = glm::normalize(glm::cross(m_forwardVec, worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_upVec = glm::normalize(glm::cross(right, m_forwardVec));

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
    }

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= m_mouseSensitivity;
        yoffset *= m_mouseSensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        updateVectors();
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
    std::string name;
    glm::vec3 m_position;
    glm::vec3 m_forwardVec;
    glm::vec3 m_upVec;

private:
    glm::mat4 m_ProjectionMatrix;
    glm::vec3 right;
    float m_yaw;
    float m_pitch;
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;
    float m_nearPlane;
    float m_farPlane;

};

#endif //LUPUSFIRE_CORE_CAMERA_H
