//
// Created by Simeon on 4/9/2024.
//

#ifndef CORE_COLLIDER_H
#define CORE_COLLIDER_H

#include "Component.h"
#include "Ray.h"
#include "Shader.h"

class Collider : public Component {
public:
    virtual bool intersectsRay(const Ray& ray, const glm::mat4& transformMatrix, float disnce) = 0;
    virtual void Draw(Shader& wireframe) = 0 ;
};

#endif