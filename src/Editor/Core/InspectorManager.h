//
// Created by Simeon on 4/8/2024.
//

#ifndef LUPUSFIRE_CORE_INSPECTORMANAGER_H
#define LUPUSFIRE_CORE_INSPECTORMANAGER_H

#include <memory>
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "Light.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
namespace IDK::Editor
{
    class InspectorManager {
    public:
        InspectorManager() = default;
        void renderInspector();

    private:
        void renderComponentObjectsInspector(const std::shared_ptr<Component>& componentObj);
        void renderEntityInspector(const std::shared_ptr<Entity> &entities);

        void renderLightInspector(const std::shared_ptr<IDK::Graphics::Light>& light);
        void renderCameraInspector(const std::shared_ptr<IDK::Graphics::Camera>& camera);
        void renderMeshInspector(const std::shared_ptr<IDK::Graphics::Mesh>& mesh);
        auto renderMaterialInspector(const std::shared_ptr<IDK::Graphics::Material>& material) -> void;
        void renderTransformComponent(const std::shared_ptr<Transform>& transform);

        bool baseColorPrinted = false;
        glm::vec3 baseColor;

    };
}
#endif //LUPUSFIRE_CORE_INSPECTORMANAGER_H
