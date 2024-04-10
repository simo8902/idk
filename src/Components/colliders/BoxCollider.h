//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_BOXCOLLIDER_H
#define LUPUSFIRE_CORE_BOXCOLLIDER_H

#include <sstream>
#include "Collider.h"
#include "Ray.h"
#include "../BoundingBox.h"

class BoxCollider : public Collider {
public:
    BoxCollider(const glm::vec3& position, const glm::vec3& min, const glm::vec3& max)
            : m_position(position), m_min(min), m_max(max) {}

    const glm::vec3& getMin() const { return m_min; }
    const glm::vec3& getMax() const { return m_max; }

    const glm::vec3 & getPosition() const {
        return m_position;
    }

    bool intersectsRay(const Ray& ray) override {
        const float epsilon = 0.0001f; // A small tolerance value

        // Calculate tmin and tmax for each axis
        float tmin = (m_min.x - ray.m_origin.x) / ray.m_direction.x;
        float tmax = (m_max.x - ray.m_origin.x) / ray.m_direction.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (m_min.y - ray.m_origin.y) / ray.m_direction.y;
        float tymax = (m_max.y - ray.m_origin.y) / ray.m_direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (m_min.z - ray.m_origin.z) / ray.m_direction.z;
        float tzmax = (m_max.z - ray.m_origin.z) / ray.m_direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        // Early-out if ray's Y-component is near-zero
        if (fabsf(ray.m_direction.y) < epsilon) {
            return false;
        }

        // Final overlap check with tolerance
        if (tmin > tmax + epsilon || tmax < -epsilon) {
            return false;
        }

        return true;
    }
    glm::vec3 m_min;
    glm::vec3 m_max;
private:
    glm::vec3 m_position;

    std::string vec3_to_string(const glm::vec3& v) const { // Make this function const
        std::ostringstream oss;
        oss << "glm::vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return oss.str();
    }
    std::shared_ptr<GameObject> m_gameObject_weak;
};

#endif //LUPUSFIRE_CORE_BOXCOLLIDER_H
