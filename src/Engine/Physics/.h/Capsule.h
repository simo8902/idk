//
// Created by Simeon on 5/9/2024.
//

#ifndef LUPUSFIRE_CORE_CAPSULE_H
#define LUPUSFIRE_CORE_CAPSULE_H

#include "GameObject.h"
#include "Mesh.h"

class Capsule : public GameObject {
public:
    Capsule(const std::string & name);
    ~Capsule() override;

    virtual std::shared_ptr<GameObject> clone() const override {
        return nullptr;
    }

    static size_t GetMemoryUsage() {
        return memoryUsage;
    }

    // Disable copying
    Capsule(const Capsule&) = delete;
    Capsule& operator=(const Capsule&) = delete;

    // Enable moving
    Capsule(Capsule&&) = default;
    Capsule& operator=(Capsule&&) = default;

    void Draw(const Shader& shader) override;

private:
    static size_t memoryUsage;
    std::unique_ptr<Mesh> mesh;
};

#endif //NAV2SFM Core_CAPSULE_H
