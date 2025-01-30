#include "SelectionManager.h"

SelectionManager& SelectionManager::getInstance() {
    static SelectionManager instance;
    return instance;
}

void SelectionManager::selectMesh(const std::shared_ptr<Mesh>& mesh) {
    selectedMesh = mesh;
    selectedMaterial.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectMaterial(const std::shared_ptr<Material>& material) {
    selectedMaterial = material;
    selectedMesh.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectComponent(const std::shared_ptr<Component>& component) {
    selectedComponent = component;
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
    selectedComponent.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectFolder(const std::shared_ptr<AssetItem>& folder) {
    selectedFolder = folder;
}

void SelectionManager::selectCamera(const std::shared_ptr<Camera>& camera) {
    selectedCamera = camera;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedShader.reset();
}

void SelectionManager::selectShader(const std::shared_ptr<Shader>& shader) {
    selectedShader = shader;
    selectedMaterial.reset();
    selectedFolder.reset();
}

void SelectionManager::clearSelection() {
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
    selectedFolder.reset();
}

std::shared_ptr<Component> SelectionManager::getSelectedComponent() const {
    return selectedComponent;
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

std::shared_ptr<AssetItem> SelectionManager::getSelectedFolder() const {
    return selectedFolder;
}