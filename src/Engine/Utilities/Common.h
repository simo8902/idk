//
// Created by SIMEON on 4/5/2025.
//

#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <iosfwd>

using EntityID = uint32_t;
constexpr size_t MAX_COMPONENTS = 64;

enum class EntityType {
    Cube, Capsule, Light,
    Cylinder, Camera, Sphere, Unknown
};

inline std::ostream& operator<<(std::ostream& os, EntityType type) {
    switch(type) {
    case EntityType::Cube:     return os << "Cube";
    case EntityType::Capsule:  return os << "Capsule";
    case EntityType::Light:    return os << "Light";
    case EntityType::Sphere:   return os << "Sphere";
    case EntityType::Cylinder: return os << "Cylinder";
    case EntityType::Camera:   return os << "Camera";
    default:                   return os << "Unknown";
    }
}
#endif //COMMON_H
