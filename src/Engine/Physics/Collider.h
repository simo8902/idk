//
// Created by Simeon on 4/9/2024.
//

#ifndef CORE_COLLIDER_H
#define CORE_COLLIDER_H

#include "../ECS/Component.h"
#include "Ray.h"
#include "Shader.h"

class Collider : public Component {
public:
    explicit Collider(const std::string& name)
     : Component(name) {}

    virtual bool intersectsRay(const Ray& ray, const glm::mat4& transformMatrix, float disnce) = 0;
    virtual void Draw(IDK::Graphics::Shader& wireframe) = 0 ;
};

#endif