#include "SelectionManager.h"

SelectionManager& SelectionManager::getInstance() {
    static SelectionManager instance;
    return instance;
}

void SelectionManager::selectMesh(const std::shared_ptr<Mesh>& mesh) {
    selectedMesh = mesh;
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectMaterial(const std::shared_ptr<Material>& material) {
    selectedMaterial = material;
    selectedMesh.reset();
    selectedGameObject.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectGameObject(const std::shared_ptr<GameObject>& object) {
    selectedGameObject = object;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectLight(const std::shared_ptr<Light>& light) {
    selectedLight = light;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectCamera(const std::shared_ptr<Camera>& camera) {
    selectedCamera = camera;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedLight.reset();
    selectedShader.reset();
}

void SelectionManager::selectShader(const std::shared_ptr<Shader>& shader) {
    selectedShader = shader;
}

void SelectionManager::clearSelection() {
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedGameObject.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

std::shared_ptr<Material> SelectionManager::getSelectedMaterial() const {
    return selectedMaterial;
}

std::shared_ptr<Mesh> SelectionManager::getSelectedMesh() const {
    return selectedMesh;
}

std::shared_ptr<Shader> SelectionManager::getSelectedShader() const {
    return selectedShader;
}