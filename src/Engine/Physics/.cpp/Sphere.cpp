//
// Created by Simeon on 7/15/2024.
//

#include ".h/Sphere.h"

size_t Sphere::memoryUsage = 0;

Sphere::Sphere(const std::string& name)
    : GameObject(name) {
    Sphere::memoryUsage += sizeof(Sphere);
    mesh = std::make_unique<Mesh>(name);
    mesh->CreateSphere(1.0f, 1024, 32);
}

Sphere::~Sphere() {
    Sphere::memoryUsage -= sizeof(Sphere);
}

void Sphere::Draw(const Shader& shader) {}