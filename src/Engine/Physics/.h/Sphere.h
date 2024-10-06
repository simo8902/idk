//
// Created by Simeon on 7/15/2024.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "GameObject.h"
#include "Mesh.h"

class Sphere : public GameObject {
public:
    Sphere(const std::string& name);
    ~Sphere() override;

    virtual std::shared_ptr<GameObject> clone() const override {
        return nullptr;
    }

    static size_t GetMemoryUsage() {
        return memoryUsage;
    }

    void Draw(const Shader& shader) override;
private:
    static size_t memoryUsage;
    std::unique_ptr<Mesh> mesh;

};


#endif //NAV2SFM Core_SPHERE_H
