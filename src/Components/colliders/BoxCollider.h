//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_BOXCOLLIDER_H
#define LUPUSFIRE_CORE_BOXCOLLIDER_H

#include "Collider.h"

class BoxCollider : public Collider {
public:
    BoxCollider(const glm::vec3& position, const glm::vec3& min, const glm::vec3& max)
        : m_position(position), m_min(min), m_max(max) {}

    const glm::vec3 getPosition(){
        return m_position;
    }

    const glm::vec3 getMin(){
        return m_min;
    }

    const glm::vec3 getMax(){
        return m_max;
    }

    bool intersects(const Ray& ray) override {
        // Ray-AABB intersection test
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

        return true;
    }
    glm::vec3 m_position, m_min, m_max;

};

#endif //LUPUSFIRE_CORE_BOXCOLLIDER_H
