//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_TRANSFORM_H
#define LUPUSFIRE_CORE_TRANSFORM_H

#include <iostream>
#include "Component.h"

namespace Utils
{
    inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
}

class Transform : public Component {
public:
    // Transform Data
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

    friend std::ostream& operator<<(std::ostream& os, const Transform& transform) {
        os << "Transform(position: ";
        Utils::operator<<(os, transform.position);
        os << ", rotation: ";
        Utils::operator<<(os, transform.rotation);
        os << ", scale: ";
        Utils::operator<<(os, transform.m_scale);
        os << ")";
        return os;
    }


};



#endif //LUPUSFIRE_CORE_TRANSFORM_H
