//
// Created by Simeon on 9/27/2024.
//

#include "Mesh.h"
#include <ext/scalar_constants.hpp>

namespace IDK::Graphics
{
    Mesh::Mesh(const Mesh& other, const std::string& newName)
        : AssetItem(
            newName,          // or: std::filesystem::path(newName).stem().string() if you like
            AssetType::Mesh,
            ""
          )
    {
        this->vertices = other.vertices;
        this->indices  = other.indices;
        this->name = newName;

        if (!this->vertices.empty()) {
            SetupMesh();
        }
    }


    Mesh::Mesh(const std::vector<float>& vertices, const std::string& name)
        : AssetItem(std::filesystem::path(name).stem().string(), AssetType::Mesh, std::filesystem::path(name).parent_path().string()),
          VAO(0), VBO(0), EBO(0) {

        if (!vertices.empty() && vertices.size() % 6 == 0) {
            size_t numVertices = vertices.size() / 6;
            this->vertices.reserve(numVertices);

            for (size_t i = 0; i < numVertices; ++i) {
                size_t baseIndex = i * 6;
                glm::vec3 pos(vertices[baseIndex], vertices[baseIndex + 1], vertices[baseIndex + 2]);
                glm::vec3 norm(vertices[baseIndex + 3], vertices[baseIndex + 4], vertices[baseIndex + 5]);
                this->vertices.push_back({pos, norm});
            }

            SetupMesh();
        } else {
            std::cerr << "Warning: Invalid vertex data provided." << std::endl;
        }
    }

    Mesh::Mesh(const std::string &name)
        : AssetItem(name, AssetType::Mesh, ""),
          name(name),
          VAO(0), VBO(0), EBO(0),
          vertices(),
          indices() {
        // std::cerr << "Mesh single param called for obj: " << name << std::endl;
    }

    Mesh::~Mesh() {
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
    }

    void Mesh::SetupMesh() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);

        if (vertices.empty()) {
            std::cerr << "Warning: SetupMesh called with no vertex data." << std::endl;
            return;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        if (!indices.empty()) {
            glGenBuffers(1, &EBO);
        }

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        if (!indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }

        // Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);
        // Normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        std::cout << "Mesh is setup for object (" << name << ")" << std::endl;
    }
    void Mesh::CreateMesh(MeshType type) {
        vertices.clear();
        indices.clear();

        switch (type) {
        case MeshType::Cube:
            CreateCube();
            break;
        case MeshType::Capsule:
            CreateCapsule(1.0f, 2.0f);
            break;
        case MeshType::Sphere:
            CreateSphere(1.0f, 20, 20);
            break;
        case MeshType::Cylinder:
            CreateCylinder(0.5f, 0.5f, 2.0f, 30);
            break;
        default:
            break;
        }

        SetupMesh();
    }
    void Mesh::Draw(const Shader& shader) const
    {
        shader.Use();
        //  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);
        if (!indices.empty()) {
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        }
        glBindVertexArray(0);

        //   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Mesh::CreateSphere(float radius, int stacks, int sectors) {
        vertices.clear();
        indices.clear();

        float stackStep = glm::pi<float>() / stacks;
        float sectorStep = 2 * glm::pi<float>() / sectors;
        float stackAngle, sectorAngle;

        // Generate vertices
        for (int i = 0; i <= stacks; ++i) {
            stackAngle = glm::pi<float>() / 2 - i * stackStep; // from pi/2 to -pi/2
            float y = radius * sin(stackAngle);
            float xy = radius * cos(stackAngle); // r * cos(theta)

            for (int j = 0; j <= sectors; ++j) {
                sectorAngle = j * sectorStep; // from 0 to 2pi

                float x = xy * cos(sectorAngle);
                float z = xy * sin(sectorAngle);
                glm::vec3 normal(x, y, z);
                normal = glm::normalize(normal);

                vertices.push_back({glm::vec3(x, y, z), normal});
            }
        }

        // Generate indices
        int k1, k2;
        for (int i = 0; i < stacks; ++i) {
            k1 = i * (sectors + 1); // beginning of current stack
            k2 = k1 + sectors + 1;  // beginning of next stack

            for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (stacks - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        std::cout << "[Sphere] Total triangles: " << (indices.size()/3)
                << " Total indices: " << indices.size() << std::endl;
    }

    void Mesh::CreateCylinder(float m_baseRadius, float m_topRadius, float m_height, int m_sectors) {
        vertices.clear();
        indices.clear();

        float sectorStep = 2 * glm::pi<float>() / m_sectors;
        float sectorAngle;

        // Generate vertices for the side
        for (int i = 0; i <= m_sectors; ++i) {
            sectorAngle = i * sectorStep;
            float x = cos(sectorAngle);
            float z = sin(sectorAngle);

            // Position (base)
            vertices.push_back({glm::vec3(x * m_baseRadius, -m_height / 2.0f, z * m_baseRadius), glm::vec3(x, 0.0f, z)});

            // Position (top)
            vertices.push_back({glm::vec3(x * m_topRadius, m_height / 2.0f, z * m_topRadius), glm::vec3(x, 0.0f, z)});
        }

        // Indices for the side
        for (int i = 0; i < m_sectors; ++i) {
            int k1 = 2 * i;
            int k2 = 2 * i + 1;
            int k3 = 2 * (i + 1);
            int k4 = 2 * (i + 1) + 1;

            // First triangle
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k3);

            // Second triangle
            indices.push_back(k2);
            indices.push_back(k4);
            indices.push_back(k3);
        }

        // Base center vertex
        unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size());
        vertices.push_back({glm::vec3(0.0f, -m_height / 2.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)});

        // Top center vertex
        unsigned int topCenterIndex = static_cast<unsigned int>(vertices.size());
        vertices.push_back({glm::vec3(0.0f, m_height / 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)});

        // Indices for the base
        for (int i = 0; i < m_sectors; ++i) {
            int k1 = 2 * i;
            int k2 = 2 * (i + 1);
            indices.push_back(baseCenterIndex);
            indices.push_back(k2);
            indices.push_back(k1);
        }

        // Indices for the top
        for (int i = 0; i < m_sectors; ++i) {
            int k1 = 2 * i + 1;
            int k2 = 2 * (i + 1) + 1;
            indices.push_back(topCenterIndex);
            indices.push_back(k1);
            indices.push_back(k2);
        }

        std::cout << "[Cylinder] triangles: " << (indices.size()/3)
              << " indices: " << indices.size() << std::endl;

    }

    void Mesh::CreateCapsule(const float& radius, const float& height) {
        vertices.clear();
        indices.clear();

        constexpr int SLICES = 32;
        constexpr int STACKS = 16;
        const float halfHeight = height * 0.5f;

        // Generate vertices for the top hemisphere
        for (int stack = 0; stack <= STACKS / 2; ++stack) {
            float phi = glm::pi<float>() * stack / (STACKS / 2);
            float y = radius * cos(phi);
            float r = radius * sin(phi);

            for (int slice = 0; slice <= SLICES; ++slice) {
                float theta = glm::two_pi<float>() * slice / SLICES;
                float x = r * cos(theta);
                float z = r * sin(theta);
                glm::vec3 normal(x, y, z);
                normal = glm::normalize(normal);
                vertices.push_back({glm::vec3(x, y + halfHeight, z), normal});
            }
        }

        // Generate vertices for the cylinder
        for (int i = 0; i <= 1; ++i) {
            float y = halfHeight * (1 - 2 * i);
            for (int slice = 0; slice <= SLICES; ++slice) {
                float theta = glm::two_pi<float>() * slice / SLICES;
                float x = radius * cos(theta);
                float z = radius * sin(theta);
                glm::vec3 normal(x, 0.0f, z);
                normal = glm::normalize(normal);
                vertices.push_back({glm::vec3(x, y, z), normal});
            }
        }

        // Generate vertices for the bottom hemisphere
        for (int stack = 0; stack <= STACKS / 2; ++stack) {
            float phi = glm::pi<float>() * stack / (STACKS / 2);
            float y = radius * cos(phi);
            float r = radius * sin(phi);

            for (int slice = 0; slice <= SLICES; ++slice) {
                float theta = glm::two_pi<float>() * slice / SLICES;
                float x = r * cos(theta);
                float z = r * sin(theta);
                glm::vec3 normal(x, -y, z);
                normal = glm::normalize(normal);
                vertices.push_back({glm::vec3(x, -y - halfHeight, z), normal});
            }
        }

        // Generate indices for the top hemisphere
        int vertsPerRing = SLICES + 1;
        for (int stack = 0; stack < STACKS / 2; ++stack) {
            for (int slice = 0; slice < SLICES; ++slice) {
                int current = stack * vertsPerRing + slice;
                int next = current + 1;
                int below = (stack + 1) * vertsPerRing + slice;
                int belowNext = below + 1;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(below);

                indices.push_back(next);
                indices.push_back(belowNext);
                indices.push_back(below);
            }
        }

        // Generate indices for the cylinder
        int offset = (STACKS / 2 + 1) * vertsPerRing;
        for (int slice = 0; slice < SLICES; ++slice) {
            int current = offset + slice;
            int next = current + 1;
            int below = offset + vertsPerRing + slice;
            int belowNext = below + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(below);

            indices.push_back(next);
            indices.push_back(belowNext);
            indices.push_back(below);
        }

        // Generate indices for the bottom hemisphere
        offset = (STACKS / 2 + 1 + 2) * vertsPerRing;
        for (int stack = 0; stack < STACKS / 2; ++stack) {
            for (int slice = 0; slice < SLICES; ++slice) {
                int current = offset + stack * vertsPerRing + slice;
                int next = current + 1;
                int below = offset + (stack + 1) * vertsPerRing + slice;
                int belowNext = below + 1;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(below);

                indices.push_back(next);
                indices.push_back(belowNext);
                indices.push_back(below);
            }
        }

        std::cout << "[Capsule] Total triangles: " << (indices.size() / 3)
                  << " Total indices: " << indices.size() << std::endl;
    }

    void Mesh::CreateCube() {
        vertices.clear();
        indices.clear();

        vertices.reserve(24); // 24 vertices (6 faces * 4 vertices per face)
        indices.reserve(36); // 36 indices (6 faces * 2 triangles per face * 3 indices per triangle)

        auto addVertex = [&](const glm::vec3& pos, const glm::vec3& normal) {
            vertices.push_back({pos, normal});
        };

        auto addFace = [&](const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& normal) {
            int startIndex = static_cast<int>(vertices.size());

            addVertex(v0, normal);
            addVertex(v1, normal);
            addVertex(v2, normal);
            addVertex(v3, normal);

            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 3);
        };

        // +Z face (front)
        addFace(
            glm::vec3(-0.5f, -0.5f, +0.5f),
            glm::vec3(+0.5f, -0.5f, +0.5f),
            glm::vec3(+0.5f, +0.5f, +0.5f),
            glm::vec3(-0.5f, +0.5f, +0.5f),
            glm::vec3(0.0f, 0.0f, +1.0f)
        );

        // -Z face (back)
        addFace(
            glm::vec3(+0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, +0.5f, -0.5f),
            glm::vec3(+0.5f, +0.5f, -0.5f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // +X face (right)
        addFace(
            glm::vec3(+0.5f, -0.5f, +0.5f),
            glm::vec3(+0.5f, -0.5f, -0.5f),
            glm::vec3(+0.5f, +0.5f, -0.5f),
            glm::vec3(+0.5f, +0.5f, +0.5f),
            glm::vec3(+1.0f, 0.0f, 0.0f)
        );

        // -X face (left)
        addFace(
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f, +0.5f),
            glm::vec3(-0.5f, +0.5f, +0.5f),
            glm::vec3(-0.5f, +0.5f, -0.5f),
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );

        // +Y face (top)
        addFace(
            glm::vec3(-0.5f, +0.5f, +0.5f),
            glm::vec3(+0.5f, +0.5f, +0.5f),
            glm::vec3(+0.5f, +0.5f, -0.5f),
            glm::vec3(-0.5f, +0.5f, -0.5f),
            glm::vec3(0.0f, +1.0f, 0.0f)
        );

        // -Y face (bottom)
        addFace(
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(+0.5f, -0.5f, -0.5f),
            glm::vec3(+0.5f, -0.5f, +0.5f),
            glm::vec3(-0.5f, -0.5f, +0.5f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        );


        std::cout << "[Cube] vertices: " << (vertices.size()/6)
                  << ", indices: " << indices.size()
                  << ", triangles: " << (indices.size()/3)
                  << std::endl;

        // std::cerr << "Init done for Cube\n";

        SetupMesh();
        std::cout << "VAO: " << VAO << ", VBO: " << VBO << ", EBO: " << EBO << std::endl;
    }
}