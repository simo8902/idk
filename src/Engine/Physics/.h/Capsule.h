//
// Created by Simeon on 5/9/2024.
//

#ifndef LUPUSFIRE_CORE_CAPSULE_H
#define LUPUSFIRE_CORE_CAPSULE_H
#include <vector>
#include <cmath>
#include "glad/glad.h"
#include "GameObject.h"

struct Vertex {
    float x, y, z;
};


class Capsule : public GameObject {
public:
    Capsule(const std::string & name);
    ~Capsule();

    virtual std::shared_ptr<GameObject> clone() const override {
        std::shared_ptr<Capsule> newCapsule = std::make_shared<Capsule>(*this); // Shallow copy

        return newCapsule;
    }

    static size_t GetMemoryUsage() {
        return memoryUsage;
    }


    void SetupMesh(float radius, float height, int resolution);
    void Draw(const Shader& shader);

private:
    std::vector<Vertex> vertices;
    GLuint VAO, VBO;
    static size_t memoryUsage;

};

#endif //LUPUSFIRE_CORE_CAPSULE_H
