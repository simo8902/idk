//
// Created by Simeon on 5/9/2024.
//

#include ".h/Capsule.h"

size_t Capsule::memoryUsage = 0;

Capsule::Capsule(const std::string &name) : GameObject(name) {
    Capsule::memoryUsage += sizeof(Capsule);
    mesh = std::make_unique<Mesh>(name);

    float radius = 1.0f;
    float height = 2.0f;
    int resolution = 512;
    mesh->CreateCapsule(radius, height, resolution);
}

Capsule::~Capsule() {
    Capsule::memoryUsage -= sizeof(Capsule);
}

void Capsule::Draw(const Shader &shader) {
}
