//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_TRANSFORM_H
#define LUPUSFIRE_CORE_TRANSFORM_H

#include <iostream>
#include "Component.h"
#include "gtx/matrix_decompose.hpp"

class Transform : public Component {
public:
    Transform() : modelMatrix(glm::mat4(1.0f)), position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), m_scale(glm::vec3(1.0f)) {}

    void setPosition(const glm::vec3& pos) { position = pos;}
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    void setScale(const glm::vec3& scale) { m_scale = scale; }

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getRotation() const { return rotation; }
    glm::vec3 getScale() const { return m_scale; }

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);

    glm::mat4 getModelMatrix() const {
        glm::mat4 transform = glm::mat4(1.0f);

        // Translate
        transform = glm::translate(transform, position);

        // Rotate
        transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        // Scale
        transform = glm::scale(transform, m_scale);

        return transform;
    }
    void setModelMatrix(const glm::mat4& matrix) {
        modelMatrix = matrix;
        glm::quat rotationQuat;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(modelMatrix, m_scale, rotationQuat, position, skew, perspective);

        // Convert the rotation quaternion to Euler angles
        rotation = glm::degrees(glm::eulerAngles(rotationQuat));

    }

    glm::mat4 modelMatrix;
};



#endif //LUPUSFIRE_CORE_TRANSFORM_H
