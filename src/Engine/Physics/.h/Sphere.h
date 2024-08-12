//
// Created by Simeon on 7/15/2024.
//

#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include "glad/glad.h"
#include "GameObject.h"

struct SphereVertex {
    float x, y, z;
};

class Sphere : public GameObject {
public:
    Sphere(const std::string& name);
    ~Sphere() override;

    virtual std::shared_ptr<GameObject> clone() const override {
        std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(*this); // Shallow copy

        return sphere;
    }

    static size_t GetMemoryUsage() {
        return memoryUsage;
    }


    void SetupMesh(float radius, int resolution);
    void Draw(const Shader& shader) override;

private:
    std::vector<SphereVertex> vertices;
    GLuint VAO, VBO;
    static size_t memoryUsage;

};


#endif //SPHERE_H
