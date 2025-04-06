//
// Created by SIMEON on 4/6/2025.
//

#ifndef MESHREGISTRY_H
#define MESHREGISTRY_H

#include <vector>
#include <memory>
#include <mutex>

namespace IDK {
    class Mesh;

    class MeshRegistry
    {
    public:
        static MeshRegistry& Instance()
        {
            static MeshRegistry instance;
            return instance;
        }

        template<typename MeshT>
        void registerMesh(const std::shared_ptr<MeshT>& mesh) {
            static_assert(std::is_base_of<IDK::Graphics::Mesh, MeshT>::value,
                         "Must register a type derived from IDK::Mesh");
            std::lock_guard<std::mutex> lock(m_mutex);
            m_meshes.push_back(std::weak_ptr<IDK::Graphics::Mesh>(mesh));
        }

        std::vector<std::weak_ptr<IDK::Graphics::Mesh>> getMeshes() const {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_meshes;
        }

    private:
        MeshRegistry() = default;
        ~MeshRegistry() = default;
        MeshRegistry(const MeshRegistry&) = delete;
        MeshRegistry& operator=(const MeshRegistry&) = delete;

        mutable std::mutex m_mutex;
        std::vector<std::weak_ptr<IDK::Graphics::Mesh>> m_meshes;
    };
}
#endif //MESHREGISTRY_H
