//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_RAY_H
#define LUPUSFIRE_CORE_RAY_H

#include "glm.hpp"
#include <iostream>
#include "gtx/string_cast.hpp"
#include "../../Camera.h"

class Ray {
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction)
            : m_origin(origin), m_direction(glm::normalize(direction)) {}

    ~Ray(){}

    const glm::vec3& getOrigin() const {
        return m_origin;
    }

    const glm::vec3& getDirection() const {
        return m_direction;
    }

    Ray getRayFromScreenPoint(glm::vec2 ndc, std::shared_ptr<Camera> m_Camera) {
        glm::vec4 rayStart_NDC(ndc.x, ndc.y, -1.0f, 1.0f);
        glm::vec4 rayEnd_NDC(ndc.x, ndc.y, 0.0f, 1.0f);

        glm::mat4 currentProjection = m_Camera->getProjectionMatrix();
        glm::mat4 currentView = m_Camera->getViewMatrix();

        glm::mat4 invProjMatrix = glm::inverse(currentProjection);
        glm::mat4 invViewMatrix = glm::inverse(currentView);

        glm::vec4 rayStart_world = invViewMatrix * invProjMatrix * rayStart_NDC;
        rayStart_world /= rayStart_world.w;
        glm::vec4 rayEnd_world = invViewMatrix * invProjMatrix * rayEnd_NDC;
        rayEnd_world /= rayEnd_world.w;

        glm::vec3 rayOrigin = m_Camera->getPosition();
        glm::vec3 rayDirection = glm::normalize(glm::vec3(rayEnd_world) - glm::vec3(rayStart_world));

        return Ray(rayOrigin, rayDirection);
    }



    glm::vec3 m_origin;
    glm::vec3 m_direction;

private:

};

#endif //LUPUSFIRE_CORE_RAY_H
