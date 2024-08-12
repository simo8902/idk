//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_BOXCOLLIDER_H
#define LUPUSFIRE_CORE_BOXCOLLIDER_H

#include <sstream>
#include "Collider.h"
#include "Ray.h"
#include "gtx/string_cast.hpp"

class BoxCollider : public Collider {
public:
    // Calculate in world space
    BoxCollider(const glm::vec3& position, const glm::vec3& min, const glm::vec3& max)
            : m_position(position), m_worldMin(min), m_worldMax(max)  {
        updateModelMatrix();
    }

    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getMin() const { return m_worldMin; }
    const glm::vec3& getMax() const { return m_worldMax; }

    std::unique_ptr<Component> clone() const override {
        return std::make_unique<BoxCollider>(*this);
    }

    void setPosition(const glm::vec3& pos) {
        m_position = pos;
    }
    void updateModelMatrix() {
        m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
    }

    const glm::mat4& getModelMatrix() const {
        return m_modelMatrix;
    }

    glm::vec3 m_worldMin;
    glm::vec3 m_worldMax;
    glm::vec3 m_position;
    glm::mat4 m_modelMatrix;

    bool intersectsRay(const Ray& ray, const glm::mat4& transformMatrix) {
        glm::mat4 invTransform = glm::inverse(transformMatrix);

        glm::vec3 rayOriginLocal = glm::vec3(invTransform * glm::vec4(ray.getOrigin(), 1.0f));
        glm::vec3 rayDirectionLocal = glm::normalize(glm::vec3(invTransform * glm::vec4(ray.getDirection(), 0.0f))); // Normalize here!

        glm::vec3 rayDirectionWorld = glm::vec3(transformMatrix * glm::vec4(rayDirectionLocal, 0.0f));
        if (glm::dot(rayDirectionWorld, m_worldMax - ray.getOrigin()) < 0) {
            std::cerr << "Error: Ray is not pointing towards the bounding box!" << std::endl;
            return false;
        }

        // Check if the ray direction is normalized
        if (glm::length(rayDirectionLocal) < 0.99f || glm::length(rayDirectionLocal) > 1.01f) {
            std::cerr << "Error: Ray direction is not normalized!" << std::endl;
            return false;
        }

        // Check bounding box dimensions
        if (m_worldMin.x >= m_worldMax.x || m_worldMin.y >= m_worldMax.y || m_worldMin.z >= m_worldMax.z) {
            std::cerr << "Error: Invalid bounding box dimensions!" << std::endl;
            return false;
        }

        if (rayDirectionLocal == glm::vec3(0.0f)) {
            std::cerr << "Error: Division by zero!" << std::endl;
            return false;
        }

        glm::vec3 invDir = 1.0f / rayDirectionLocal;
        glm::vec3 t0s = (m_worldMin - rayOriginLocal) * invDir;
        glm::vec3 t1s = (m_worldMax - rayOriginLocal) * invDir;

        glm::vec3 tmin = glm::min(t0s, t1s);
        glm::vec3 tmax = glm::max(t0s, t1s);

        float tMin = std::max(tmin.x, std::max(tmin.y, tmin.z));
        float tMax = std::min(tmax.x, std::min(tmax.y, tmax.z));

        if (tMax < 0 || tMin > tMax) {
            //std::cerr << "No valid intersection\n";
            return false;
        }
        return true;
    }
};

#endif //LUPUSFIRE_CORE_BOXCOLLIDER_H
