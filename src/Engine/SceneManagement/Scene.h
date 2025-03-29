//
// Created by Simeon on 4/5/2024.
//

#ifndef Core_SCENE_H
#define Core_SCENE_H

#include "AssetItem.h"
#include "Camera.h"
#include "Entity.h"
#include "LightManager.h"
#include "GameObject.h"
#include "Shader.h"

class Scene {
public:
    Scene(const std::shared_ptr<Camera> & camera);
    ~Scene();

    std::vector<std::shared_ptr<Entity>> components;

    const std::vector<std::shared_ptr<Entity>>& getComponents() const;

    void createObjects();

    void setCamera(const std::shared_ptr<Camera> & cam) {
        m_Camera = cam;
    }

    void DrawGrid(float gridSize, float gridStep) const;

    void setupSky();
    void renderSky();
    static std::vector<std::shared_ptr<Light>> lights;

    std::shared_ptr<LightManager> getLightManager() const {
        return lightManager;
    }

    std::shared_ptr<AssetItem> CreateGameObjectAssetItem(const std::shared_ptr<GameObject>& obj) {
        /*
        if (!obj) {
            std::cerr << "CreateGameObjectAssetItem: GameObject is null!" << std::endl;
            return nullptr;
        }
        auto assetItem = std::make_shared<AssetItem>(obj->getName(), AssetType::GameObject, "", true);
        if (!assetItem) {
            std::cerr << "CreateGameObjectAssetItem: Failed to create AssetItem!" << std::endl;
            return nullptr;
        }
        assetItem->setGameObject(obj);
        std::cerr << "Created GameObject AssetItem: " << assetItem->getName() << std::endl;
        return assetItem;*/
        return nullptr;
    }

private:
    GLuint skyboxTexture;
    GLuint skyVAO, skyVBO;

    std::shared_ptr<Shader> finalPassShader;

    Transform gridTransform;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;

    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<LightManager> lightManager;
};

#endif