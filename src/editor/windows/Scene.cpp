//
// Created by Simeon on 4/5/2024.
//

#include "Scene.h"

Scene* Scene::globalScene = nullptr;

Scene::Scene(): m_camera(nullptr), m_shader(nullptr) {}

Scene::~Scene() {}

void Scene::setShader(Shader& shader) {
    m_shader = &shader;
}

void Scene::setCamera(Camera& camera) {
    m_camera = &camera;
}

void Scene::setScene(Scene& scene) {
    globalScene = &scene;
}

