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

namespace IDK
{
    class Scene {
    public:
        using Ptr = std::shared_ptr<Scene>;

        Scene(const std::shared_ptr<IDK::Graphics::Camera> & camera);
        ~Scene();

        std::vector<std::shared_ptr<Entity>> components;

        const std::vector<std::shared_ptr<Entity>>& getComponents() const;

        void createObjects();

        void setCamera(const std::shared_ptr<IDK::Graphics::Camera> & cam) {
            m_Camera = cam;
        }

        void DrawGrid(float gridSize, float gridStep) const;

        void setupSky();
        void renderSky();
        static std::vector<std::shared_ptr<IDK::Graphics::Light>> lights;

        std::shared_ptr<LightManager> getLightManager() const {
            return lightManager;
        }

    private:
        GLuint skyboxTexture;
        GLuint skyVAO, skyVBO;

        std::shared_ptr<IDK::Graphics::Shader> finalPassShader;

        Transform gridTransform;
        std::vector<std::shared_ptr<DirectionalLight>> directionalLights;

        std::shared_ptr<IDK::Graphics::Camera> m_Camera;
        std::shared_ptr<LightManager> lightManager;
    };
}
#endif