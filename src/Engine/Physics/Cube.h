//
// Created by Simeon-PC on 3/28/2024.
//

#ifndef CUBE_H
#define CUBE_H

#include "BoxCollider.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "AssetManager.h"
#include "MeshRegistry.h"
#include "Registry.h"

class Cube final {
public:
    explicit Cube(const std::string& name)
    {
        m_entity = Registry::instance().createEntity(name, EntityType::Cube);
        initialize();

    }
    std::shared_ptr<Entity>  getEntity() const { return m_entity; }

private:
    std::shared_ptr<Entity>  m_entity;

    void initialize() const
    {
        auto& transform = m_entity->addComponent<Transform>();
        transform.position = glm::vec3(0.0f, 1.5f, 0.0f);

        auto& meshFilter = m_entity->addComponent<IDK::Components::MeshFilter>();
       // auto cubeMesh = std::make_shared<Mesh>("CubeMesh");
        auto cubeMesh = std::allocate_shared<IDK::Graphics::Mesh>(IDK::MeshSharedAllocator<IDK::Graphics::Mesh>(), "CubeMesh");


        cubeMesh->CreateMesh(IDK::Graphics::MeshType::Cube);
        meshFilter.setMesh(cubeMesh);
        cubeMesh->printMemUsage();

        IDK::MeshRegistry::Instance().registerMesh(cubeMesh);

        m_entity->addComponent<IDK::Components::MeshRenderer>([&](IDK::Components::MeshRenderer &mr) {
            mr.setMeshFilter(meshFilter.shared_from_this());
        });

        m_entity->addComponent<BoxCollider>(
            transform.position,
            glm::vec3(-0.6f),
            glm::vec3(0.6f)
        );
    }
};

#endif
