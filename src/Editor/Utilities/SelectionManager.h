//
// Created by Simeon on 10/1/2024.
//

#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <../../Engine/ECS/Component.h>
#include <memory>
#include "AssetItem.h"
#include "Entity.h"
#include "SelectionEvent.h"

namespace IDK::Graphics
{
    class Mesh;
    class Material;
    class Light;
    class Camera;
    class Shader;
}

class SelectionManager {
public:
    static SelectionManager& getInstance();

    void select(const std::shared_ptr<Entity>& object);
    void deselect();
    void clearSelection();
    std::shared_ptr<Entity> getSelectedObject() const;
    void registerListener(std::function<void(const SelectionEvent&)> listener);

    void selectMesh(const std::shared_ptr<IDK::Graphics::Mesh>& mesh);
    void selectMaterial(const std::shared_ptr<IDK::Graphics::Material>& material);
    void selectLight(const std::shared_ptr<IDK::Graphics::Light>& light);
    void selectCamera(const std::shared_ptr<IDK::Graphics::Camera>& camera);
    void selectShader(const std::shared_ptr<IDK::Graphics::Shader>& shader);
    void selectFolder(const std::shared_ptr<AssetItem>& folder);

    void selectVirtualAsset(const std::shared_ptr<AssetItem>& asset);

    std::shared_ptr<IDK::Graphics::Material> getSelectedMaterial() const;
    std::shared_ptr<IDK::Graphics::Mesh> getSelectedMesh() const;
    std::shared_ptr<IDK::Graphics::Shader> getSelectedShader() const;
    std::shared_ptr<AssetItem> getSelectedFolder() const;
    std::shared_ptr<Entity> getSelectedEntity() const;

    std::shared_ptr<IDK::Graphics::Mesh> selectedMesh;
    std::shared_ptr<IDK::Graphics::Material> selectedMaterial;
    std::shared_ptr<IDK::Graphics::Light> selectedLight;
    std::shared_ptr<IDK::Graphics::Camera> selectedCamera;
    std::shared_ptr<IDK::Graphics::Shader> selectedShader;
    std::shared_ptr<AssetItem> selectedFolder;

    void selectComponent(const std::shared_ptr<Entity>& component);
    const std::shared_ptr<Entity>& getSelectedComponent();
    std::shared_ptr<Entity> selectedComponent;

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

    std::shared_ptr<IDK::Graphics::Light> getSelectedLight() const { return selectedLight; }
    std::shared_ptr<IDK::Graphics::Camera> getSelectedCamera() const { return selectedCamera; }

    void clearSelections() {}

private:
    SelectionManager() = default;
    SelectionManager(const SelectionManager&) = delete;
    SelectionManager& operator=(const SelectionManager&) = delete;

    std::shared_ptr<Entity> selectedEntity;
    std::vector<std::function<void(const SelectionEvent&)>> listeners;
    void clearSpecificSelections();

    void notifySelectionChange(const SelectionEvent& event) {
        for (auto& listener : listeners) {
            listener(event);
        }
    }
};


#endif //SELECTIONMANAGER_H
