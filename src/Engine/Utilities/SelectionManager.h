//
// Created by Simeon on 10/1/2024.
//

#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <memory>

class Mesh;
class Material;
class GameObject;
class Light;
class Camera;

class SelectionManager {
public:
    static SelectionManager& getInstance() {
        static SelectionManager instance;
        return instance;
    }

    void selectMesh(const std::shared_ptr<Mesh>& mesh);
    void selectMaterial(const std::shared_ptr<Material>& material);
    void selectGameObject(const std::shared_ptr<GameObject>& object);
    void selectLight(const std::shared_ptr<Light>& light);
    void selectCamera(const std::shared_ptr<Camera>& camera);
    void clearSelection();

    std::shared_ptr<Mesh> selectedMesh;
    std::shared_ptr<Material> selectedMaterial;
    std::shared_ptr<GameObject> selectedGameObject;
    std::shared_ptr<Light> selectedLight;
    std::shared_ptr<Camera> selectedCamera;

private:
    SelectionManager() = default;
};


#endif //SELECTIONMANAGER_H
