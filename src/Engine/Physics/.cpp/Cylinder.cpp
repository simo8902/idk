//
// Created by Simeon on 7/14/2024.
//

#include ".h/Cylinder.h"
#include <cmath>

#include "Mesh.h"

Cylinder::Cylinder(const std::string& name, float baseRadius, float topRadius, float height, int sectors)
    : GameObject(name), m_baseRadius(baseRadius), m_topRadius(topRadius), m_height(height), m_sectors(sectors)
{
    mesh = std::make_unique<Mesh>(name);
    mesh->CreateCylinder(m_baseRadius, m_topRadius, m_height, m_sectors);
}

std::shared_ptr<GameObject> Cylinder::clone() const {
    return std::make_shared<Cylinder>(*this);
}

void Cylinder::Draw(const Shader& shader) {
}
