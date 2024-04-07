//
// Created by Simeon on 4/7/2024.
//

#ifndef LUPUSFIRE_CORE_BOUNDINGBOX_H
#define LUPUSFIRE_CORE_BOUNDINGBOX_H


#include "Component.h"

class BoundingBox : public Component {
public:
    glm::vec3 min; // Minimum extent
    glm::vec3 max; // Maximum extent

    BoundingBox(const glm::vec3& min = glm::vec3(0.0f), const glm::vec3& max = glm::vec3(0.0f))
            : min(min), max(max) {}

    // Check if a point is inside the bounding box
    bool contains(const glm::vec3& point) const {
        return point.x >= min.x && point.y >= min.y && point.z >= min.z &&
               point.x <= max.x && point.y <= max.y && point.z <= max.z;
    }

    // Other methods...
};


#endif //LUPUSFIRE_CORE_BOUNDINGBOX_H
