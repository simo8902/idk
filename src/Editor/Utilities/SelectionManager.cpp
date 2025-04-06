#include "SelectionManager.h"

#include "Cube.h"
#include "Entity.h"
#include "Light.h"

SelectionManager& SelectionManager::getInstance() {
    static SelectionManager instance;
    return instance;
}

void SelectionManager::select(const std::shared_ptr<Entity>& object) {
    if (!object) return;

    if (selectedEntity) {
        selectedEntity->deselect();
    }
    selectedEntity = object;
    selectedEntity->select();

    clearSpecificSelections();

    auto components = object->getAllComponents();
    for (const auto& [type, component] : components) {
        if (auto light = std::dynamic_pointer_cast<IDK::Graphics::Light>(component)) {
            selectedLight = light;
        } else if (auto camera = std::dynamic_pointer_cast<IDK::Graphics::Camera>(component)) {
            selectedCamera = camera;
        }
    }
    notifySelectionChange(SelectionEvent(SelectionEvent::Type::SELECT, object));
}
void SelectionManager::clearSpecificSelections() {
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
    selectedFolder.reset();
}
void SelectionManager::deselect() {

    if (selectedEntity) {
        selectedEntity->deselect();
        notifySelectionChange(SelectionEvent(SelectionEvent::Type::DESELECT, selectedEntity));
        selectedEntity.reset();
    }
}

void SelectionManager::clearSelection() {
    deselect();
    clearSpecificSelections();
    notifySelectionChange(SelectionEvent(SelectionEvent::Type::CLEAR, nullptr));
}

std::shared_ptr<Entity> SelectionManager::getSelectedObject() const {
    return selectedEntity;
}

void SelectionManager::registerListener(std::function<void(const SelectionEvent&)> listener) {
    listeners.push_back(listener);
}


void SelectionManager::selectMesh(const std::shared_ptr<IDK::Graphics::Mesh>& mesh) {
    selectedMesh = mesh;
    selectedMaterial.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectMaterial(const std::shared_ptr<IDK::Graphics::Material>& material) {
    selectedMaterial = material;
    selectedMesh.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectComponent(const std::shared_ptr<Entity>& component) {
    selectedComponent = component;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
}

void SelectionManager::selectLight(const std::shared_ptr<IDK::Graphics::Light>& light) {
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

void SelectionManager::selectCamera(const std::shared_ptr<IDK::Graphics::Camera>& camera) {
    selectedCamera = camera;
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedShader.reset();
}

void SelectionManager::selectShader(const std::shared_ptr<IDK::Graphics::Shader>& shader) {
    selectedShader = shader;
    selectedMaterial.reset();
    selectedFolder.reset();
}

/*
void SelectionManager::clearSelection() {
    selectedMesh.reset();
    selectedMaterial.reset();
    selectedComponent.reset();
    selectedLight.reset();
    selectedCamera.reset();
    selectedShader.reset();
    selectedFolder.reset();
}*/

const std::shared_ptr<Entity>& SelectionManager::getSelectedComponent() {
    return selectedComponent;
}

std::shared_ptr<IDK::Graphics::Material> SelectionManager::getSelectedMaterial() const {
    return selectedMaterial;
}

std::shared_ptr<IDK::Graphics::Mesh> SelectionManager::getSelectedMesh() const {
    return selectedMesh;
}

std::shared_ptr<IDK::Graphics::Shader> SelectionManager::getSelectedShader() const {
    return selectedShader;
}

std::shared_ptr<AssetItem> SelectionManager::getSelectedFolder() const {
    return selectedFolder;
}

std::shared_ptr<Entity> SelectionManager::getSelectedEntity() const {
    return selectedEntity;
}

void SelectionManager::selectVirtualAsset(const std::shared_ptr<AssetItem>& asset) {
    if (asset->getType() == AssetType::Mesh && asset->getName() == "MyCube") {
        /*
        auto cube = std::dynamic_pointer_cast<Cube>(asset);
        if (cube) {
            // Do something with the Cube
            std::cout << "Selected Cube: " << cube->getName() << std::endl;
        }*/
    }
}
