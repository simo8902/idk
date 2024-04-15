//
// Created by Simeon on 4/5/2024.
//

#include <sstream>
#include "Scene.h"
#include "imgui.h"

#include "gtx/string_cast.hpp"

using Utils::operator<<;
Scene* Scene::globalScene = nullptr;

Scene::Scene(): m_camera(nullptr), m_shader(nullptr) {
    m_gizmo = std::make_shared<Gizmo>();

}

Scene::~Scene() {

}

void Scene::setShader(Shader& shader) {
    m_shader = &shader;
}

void Scene::setCamera(Camera& camera) {
    m_camera = &camera;
}


void Scene::setScene(Scene& scene) {
    globalScene = &scene;
}

