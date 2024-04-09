//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_RAY_H
#define LUPUSFIRE_CORE_RAY_H

#include "glm.hpp"

class Ray {
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction)
            : m_origin(origin), m_direction(glm::normalize(direction)) {}

    const glm::vec3& getOrigin() const { return m_origin; }
    const glm::vec3& getDirection() const { return m_direction; }
    glm::vec3 m_origin;
    glm::vec3 m_direction;
private:

};

#endif //LUPUSFIRE_CORE_RAY_H
