//
// Created by Simeon on 10/1/2024.
//

#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <Component.h>
#include <memory>
#include "AssetItem.h"

class Mesh;
class Material;
class GameObject;
class Light;
class Camera;
class Shader;

class SelectionManager {
public:
    static SelectionManager& getInstance();

    void selectMesh(const std::shared_ptr<Mesh>& mesh);
    void selectMaterial(const std::shared_ptr<Material>& material);
    void selectLight(const std::shared_ptr<Light>& light);
    void selectCamera(const std::shared_ptr<Camera>& camera);
    void selectShader(const std::shared_ptr<Shader>& shader);
    void selectFolder(const std::shared_ptr<AssetItem>& folder);

    void clearSelection();

    std::shared_ptr<Material> getSelectedMaterial() const;
    std::shared_ptr<Mesh> getSelectedMesh() const;
    std::shared_ptr<Shader> getSelectedShader() const;
    std::shared_ptr<AssetItem> getSelectedFolder() const;

    std::shared_ptr<Mesh> selectedMesh;
    std::shared_ptr<Material> selectedMaterial;
    std::shared_ptr<GameObject> selectedGameObject;
    std::shared_ptr<Light> selectedLight;
    std::shared_ptr<Camera> selectedCamera;
    std::shared_ptr<Shader> selectedShader;
    std::shared_ptr<AssetItem> selectedFolder;

    void selectComponent(const std::shared_ptr<Component>& component);
    std::shared_ptr<Component> getSelectedComponent() const;

    bool isItemSelected(const std::shared_ptr<AssetItem>& item) const {
        if (!item) return false;
        return true;
    }

    void toggleSelectItem(const std::shared_ptr<AssetItem>& item) {
        
        if (!item) return;

        /*
        auto uuid = item->getUUID();
        if (selectedItems.find(uuid) != selectedItems.end()) {
            selectedItems.erase(uuid);
        } else {
            selectedItems.insert(uuid);
        }*/
    }

    std::shared_ptr<Light> getSelectedLight() const { return selectedLight; }
    std::shared_ptr<Camera> getSelectedCamera() const { return selectedCamera; }

    void clearSelections() {}

    std::shared_ptr<Component> selectedComponent;
private:
    SelectionManager() = default;
    SelectionManager(const SelectionManager&) = delete;
    SelectionManager& operator=(const SelectionManager&) = delete;

};


#endif //SELECTIONMANAGER_H
