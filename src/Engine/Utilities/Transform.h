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
    Transform()
            : modelMatrix(glm::mat4(1.0f)), position(glm::vec3(0.0f)),
              rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
              m_scale(glm::vec3(1.0f)) {}

    std::unique_ptr<Component> clone() const override {
        return std::make_unique<Transform>(*this);
    }

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setRotation(const glm::quat& rot) { rotation = rot; }
    void setScale(const glm::vec3& scale) { m_scale = scale; }

    glm::vec3 getPosition() const { return position; }
    glm::quat getRotation() const { return rotation; }
    glm::vec3 getScale() const { return m_scale; }

    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation;
    glm::vec3 m_scale = glm::vec3(1.0f);

    glm::vec3 getForward() const {
        return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 getBoundingBox() const {
        return position;
    }

    glm::mat4 getModelMatrix() const {
        glm::mat4 transform = glm::mat4(1.0f);

        transform = glm::translate(transform, position);
        transform *= glm::mat4_cast(rotation);
        transform = glm::scale(transform, m_scale);

        return transform;
    }

    glm::mat4 getRotationMatrix() const {
        return glm::mat4_cast(rotation);
    }

    void setModelMatrix(const glm::mat4& matrix) {
        modelMatrix = matrix;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(modelMatrix, m_scale, rotation, position, skew, perspective);

        // Ensure quaternion is normalized after decomposition
        rotation = glm::normalize(rotation);

    }

    glm::mat4 modelMatrix;
};



#endif //NAV2SFM Core_TRANSFORM_H
