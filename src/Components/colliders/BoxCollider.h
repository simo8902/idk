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
    BoxCollider(const glm::vec3& position, const glm::vec3& min, const glm::vec3& max)
            : m_position(position), m_worldMin(position + min), m_worldMax(position + max) {} // Calculate in world space

    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getMin() const { return m_worldMin; }
    const glm::vec3& getMax() const { return m_worldMax; }

    bool intersectsRay(const Ray& ray, const glm::mat4& transformMatrix) override {
        glm::mat4 invTransform = glm::inverse(transformMatrix);
        glm::vec3 rayOriginLocal = glm::vec3(invTransform * glm::vec4(ray.getOrigin(), 1.0));
        glm::vec3 rayDirectionLocal = glm::normalize(glm::vec3(invTransform * glm::vec4(ray.getDirection(), 0.0)));

        Ray transformedRay(rayOriginLocal, rayDirectionLocal);

        glm::vec3 invDir = 1.0f / transformedRay.getDirection();
        glm::vec3 t0s = (m_worldMin - transformedRay.getOrigin()) * invDir;
        glm::vec3 t1s = (m_worldMax - transformedRay.getOrigin()) * invDir;

        glm::vec3 tmin = glm::min(t0s, t1s);
        glm::vec3 tmax = glm::max(t0s, t1s);

        float tMin = std::max(tmin.x, std::max(tmin.y, tmin.z));
        float tMax = std::min(tmax.x, std::min(tmax.y, tmax.z));

        std::cout << "Ray Origin (Local): " << glm::to_string(rayOriginLocal) << "\n";
        std::cout << "Ray Direction (Local): " << glm::to_string(rayDirectionLocal) << "\n";
        std::cout << "tMin: " << tMin << ", tMax: " << tMax << "\n";
        std::cout << "Intersects: " << (tMin <= tMax && tMax >= 0 ? "Yes" : "No") << "\n";

        // Check if the ray intersects the AABB
        if (tMax < 0 || tMin > tMax) {
            return false; // No valid intersection
        }

        // The ray intersects the box if tMin is within the range [0, tMax]
        return tMin <= tMax && tMax >= 0;
    }

    glm::vec3 m_worldMin; // World space min bound
    glm::vec3 m_worldMax; // World space max bound

private:
    glm::vec3 m_position;
    std::shared_ptr<GameObject> m_gameObject_weak;
};

#endif //LUPUSFIRE_CORE_BOXCOLLIDER_H
