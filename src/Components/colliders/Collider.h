//
// Created by Simeon on 4/9/2024.
//

#ifndef LUPUSFIRE_CORE_COLLIDER_H
#define LUPUSFIRE_CORE_COLLIDER_H


#include "../Component.h"
#include "Ray.h"

class Collider : public Component {
public:
    // Check if a ray intersects with this collider
    virtual bool intersects(const Ray& ray) = 0;
};


#endif //LUPUSFIRE_CORE_COLLIDER_H
