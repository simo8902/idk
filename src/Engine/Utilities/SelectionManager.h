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
class Shader;

class SelectionManager {
public:
    static SelectionManager& getInstance();

    void selectMesh(const std::shared_ptr<Mesh>& mesh);
    void selectMaterial(const std::shared_ptr<Material>& material);
    void selectGameObject(const std::shared_ptr<GameObject>& object);
    void selectLight(const std::shared_ptr<Light>& light);
    void selectCamera(const std::shared_ptr<Camera>& camera);
    void selectShader(const std::shared_ptr<Shader>& shader);

    void clearSelection();

    std::shared_ptr<Material> getSelectedMaterial() const;
    std::shared_ptr<Mesh> getSelectedMesh() const;
    std::shared_ptr<Shader> getSelectedShader() const;

    std::shared_ptr<Mesh> selectedMesh;
    std::shared_ptr<Material> selectedMaterial;
    std::shared_ptr<GameObject> selectedGameObject;
    std::shared_ptr<Light> selectedLight;
    std::shared_ptr<Camera> selectedCamera;
    std::shared_ptr<Shader> selectedShader;
private:
    SelectionManager() = default;
    SelectionManager(const SelectionManager&) = delete;
    SelectionManager& operator=(const SelectionManager&) = delete;


};


#endif //SELECTIONMANAGER_H
