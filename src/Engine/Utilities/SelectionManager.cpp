//
// Created by Simeon on 10/1/2024.
//

#include "SelectionManager.h"

void SelectionManager::selectMesh(const std::shared_ptr<Mesh>& mesh) {
    selectedMesh = mesh;
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedLight.reset();
    selectedCamera.reset();
}

void SelectionManager::selectMaterial(const std::shared_ptr<Material>& material) {
    selectedMaterial = material;
    selectedMesh.reset();
    selectedGameObject.reset();
    selectedLight.reset();
    selectedCamera.reset();
}

void SelectionManager::selectGameObject(const std::shared_ptr<GameObject>& object) {
    selectedGameObject = object;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedLight.reset();
    selectedCamera.reset();
}

void SelectionManager::selectLight(const std::shared_ptr<Light>& light) {
    selectedLight = light;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedCamera.reset();
}

void SelectionManager::selectCamera(const std::shared_ptr<Camera>& camera) {
    selectedCamera = camera;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedLight.reset();
}

void SelectionManager::clearSelection() {
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedLight.reset();
    selectedCamera.reset();
}