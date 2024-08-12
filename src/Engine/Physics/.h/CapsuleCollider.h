//
// Created by Simeon on 5/9/2024.
//

#ifndef LUPUSFIRE_CORE_CAPSULECOLLIDER_H
#define LUPUSFIRE_CORE_CAPSULECOLLIDER_H

#include <memory>
#include "Collider.h"
#include "Ray.h"
#include "Shader.h"
#include "glad/glad.h"
#include "gtx/norm.hpp"

class CapsuleCollider : public Collider {
public:
    CapsuleCollider(const glm::vec3 &position, const float & r, const float & h)
        : m_position(position), radius(r), height(h)
    {
        capsuleCollider = this;
        wireframeScale = 1.05f;
        updateModelMatrix();

    }

    ~CapsuleCollider() override {}

    std::unique_ptr<Component> clone() const override {
        return std::make_unique<CapsuleCollider>(*this);
    }

    void updateModelMatrix() {
        m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
    }

    void Draw(Shader &wireframe) override {
        if(capsuleCollider) {
            wireframe.Use();
            wireframe.setVec3("m_wireframeColor", glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 scaledModelMatrix = glm::scale(m_modelMatrix, glm::vec3(wireframeScale));
            drawWireframeCapsule(scaledModelMatrix, radius, height);
        }
    }

    bool intersectsRay(const Ray &ray, const glm::mat4 &transformMatrix, float distance) override {
        glm::mat4 inverseTransform = glm::inverse(transformMatrix);
        glm::vec3 localRayOrigin = glm::vec3(inverseTransform * glm::vec4(ray.m_origin, 1.0f));
        glm::vec3 localRayDirection = glm::normalize(glm::vec3(inverseTransform * glm::vec4(ray.m_direction, 0.0f)));

        glm::vec3 cylinderBottom = m_position - glm::vec3(0.0f, height / 2.0f, 0.0f);
        glm::vec3 cylinderTop = m_position + glm::vec3(0.0f, height / 2.0f, 0.0f);

        float t0, t1;
        // Check intersection with the cylindrical part
        if (intersectsCylinder(localRayOrigin, localRayDirection, cylinderBottom, cylinderTop, radius, t0, t1)) {
            if ((t0 >= 0.0f && t0 <= distance) || (t1 >= 0.0f && t1 <= distance)) {
                return true;
            }
        }

        // Check intersection with the bottom hemisphere
        float t;
        if (intersectsSphere(localRayOrigin, localRayDirection, cylinderBottom, radius, t)) {
            if (t >= 0.0f && t <= distance) {
                return true;
            }
        }

        // Check intersection with the top hemisphere
        if (intersectsSphere(localRayOrigin, localRayDirection, cylinderTop, radius, t)) {
            if (t >= 0.0f && t <= distance) {
                return true;
            }
        }

        return false;

    }

private:
    glm::vec3 m_position;
    glm::mat4 m_modelMatrix;

    float wireframeScale;
    float radius, height;
    CapsuleCollider *capsuleCollider;

    bool intersectsCylinder(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection,
                            const glm::vec3 &cylinderBottom, const glm::vec3 &cylinderTop,
                            float cylinderRadius, float &t0, float &t1) const {
        glm::vec3 ba = cylinderTop - cylinderBottom;
        glm::vec3 oc = rayOrigin - cylinderBottom;

        float baba = glm::dot(ba, ba);
        float bard = glm::dot(ba, rayDirection);
        float baoc = glm::dot(ba, oc);

        float k2 = baba - bard * bard;
        float k1 = baba * glm::dot(oc, rayDirection) - baoc * bard;
        float k0 = baba * glm::dot(oc, oc) - baoc * baoc - cylinderRadius * cylinderRadius * baba;

        float discriminant = k1 * k1 - k2 * k0;
        if (discriminant < 0.0f) return false;

        float sqrtDiscriminant = sqrt(discriminant);
        t0 = (-k1 - sqrtDiscriminant) / k2;
        t1 = (-k1 + sqrtDiscriminant) / k2;

        if (t0 > t1) std::swap(t0, t1);

        glm::vec3 hit0 = rayOrigin + t0 * rayDirection;
        glm::vec3 hit1 = rayOrigin + t1 * rayDirection;

        float y0 = glm::dot(hit0 - cylinderBottom, ba) / glm::length(ba);
        float y1 = glm::dot(hit1 - cylinderBottom, ba) / glm::length(ba);

        float minY = 0.0f;
        float maxY = glm::length(ba);

        if (y0 < minY) {
            if (y1 < minY) return false;
            t0 = t0 + (t1 - t0) * (minY - y0) / (y1 - y0);
            y0 = minY;
        } else if (y0 > maxY) {
            if (y1 > maxY) return false;
            t0 = t0 + (t1 - t0) * (maxY - y0) / (y1 - y0);
            y0 = maxY;
        }

        if (y1 < minY) {
            t1 = t0 + (t1 - t0) * (minY - y0) / (y1 - y0);
            y1 = minY;
        } else if (y1 > maxY) {
            t1 = t0 + (t1 - t0) * (maxY - y0) / (y1 - y0);
            y1 = maxY;
        }

        return t0 >= 0.0f || t1 >= 0.0f;
    }

    bool intersectsSphere(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection,
                              const glm::vec3 &sphereCenter, float sphereRadius, float &t) const {
        glm::vec3 oc = rayOrigin - sphereCenter;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(oc, rayDirection);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return false;

        float sqrtDiscriminant = sqrt(discriminant);
        t = (-b - sqrtDiscriminant) / (2.0f * a);
        if (t < 0.0f) {
            t = (-b + sqrtDiscriminant) / (2.0f * a);
            if (t < 0.0f) return false;
        }
        return true;
    }


    static void drawWireframeHemisphere(const glm::mat4 &transformMatrix, const glm::vec3 &center, float radius, bool isTop) {
        const int segments = 16;
        const int rings = 8; // half the rings for a hemisphere

        for (int i = 0; i <= rings; ++i) {
            float lat0 = (isTop ? 0.5f : -0.5f) * glm::pi<float>() + glm::pi<float>() * i / (2 * rings);
            float lat1 = (isTop ? 0.5f : -0.5f) * glm::pi<float>() + glm::pi<float>() * (i + 1) / (2 * rings);

            float z0 = sin(lat0) * radius, zr0 = cos(lat0) * radius;
            float z1 = sin(lat1) * radius, zr1 = cos(lat1) * radius;

            glBegin(GL_LINE_LOOP);
            for (int j = 0; j <= segments; ++j) {
                float lng = 2 * glm::pi<float>() * j / segments;
                float x = cos(lng), y = sin(lng);

                glm::vec4 v0 = transformMatrix * glm::vec4(center + glm::vec3(x * zr0, y * zr0, z0), 1.0f);
                glm::vec4 v1 = transformMatrix * glm::vec4(center + glm::vec3(x * zr1, y * zr1, z1), 1.0f);

                glVertex3f(v0.x, v0.y, v0.z);
                glVertex3f(v1.x, v1.y, v1.z);
            }
            glEnd();
        }
    }

    static void drawWireframeCapsule(const glm::mat4 &transformMatrix, float radius, float height) {
        const int segments = 16;
        glm::vec3 base(0.0f, 0.0f, 0.0f);
        glm::vec3 top(0.0f, height, 0.0f);

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * glm::pi<float>() * i / segments;
            float x = radius * cos(theta), z = radius * sin(theta);

            glm::vec4 vBase = transformMatrix * glm::vec4(base.x + x, base.y, base.z + z, 1.0f);
            glm::vec4 vTop = transformMatrix * glm::vec4(top.x + x, top.y, top.z + z, 1.0f);

            glVertex3f(vBase.x, vBase.y, vBase.z);
            glVertex3f(vTop.x, vTop.y, vTop.z);
        }
        glEnd();

        glBegin(GL_LINES);
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * glm::pi<float>() * i / segments;
            float x = radius * cos(theta), z = radius * sin(theta);

            glm::vec4 vBase = transformMatrix * glm::vec4(base.x + x, base.y, base.z + z, 1.0f);
            glm::vec4 vTop = transformMatrix * glm::vec4(top.x + x, top.y, top.z + z, 1.0f);

            glVertex3f(vBase.x, vBase.y, vBase.z);
            glVertex3f(vTop.x, vTop.y, vTop.z);
        }
        glEnd();

        drawWireframeHemisphere(transformMatrix, base, radius, false);
        drawWireframeHemisphere(transformMatrix, top, radius, true);
    }





};
#endif //LUPUSFIRE_CORE_CAPSULECOLLIDER_H
