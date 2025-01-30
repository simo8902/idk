//
// Created by Simeon on 9/27/2024.
//

#include "Mesh.h"

#include <ext/scalar_constants.hpp>

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
      VAO(0), VBO(0), EBO(0),
      vertices(vertices)
{
    if (!vertices.empty()) {
        SetupMesh();
    }
}

Mesh::Mesh(const std::string &name)
    : AssetItem(std::filesystem::path(name).stem().string(), AssetType::Mesh, std::filesystem::path(name).parent_path().string()),
      VAO(0), VBO(0), EBO(0) {}

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

    // Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Element Buffer
    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) const
{
    shader.Use();
  //  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(VAO);
    if (!indices.empty()) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));
    }

    glBindVertexArray(0);

 //   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Mesh::CreateSphere(float radius, int stacks, int sectors) {
    vertices.clear();
    indices.clear();

    float x, y, z, nx, ny, nz;
    float stackStep = glm::pi<float>() / stacks;
    float sectorStep = 2 * glm::pi<float>() / sectors;
    float stackAngle, sectorAngle;

    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        stackAngle = glm::pi<float>() / 2 - i * stackStep; // from pi/2 to -pi/2
        y = radius * sin(stackAngle);
        float xy = radius * cos(stackAngle); // r * cos(theta)

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep; // from 0 to 2pi

            x = xy * cos(sectorAngle);
            z = xy * sin(sectorAngle);
            nx = x / radius;
            ny = y / radius;
            nz = z / radius;

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            // Normal
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
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

    SetupMesh();

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
        vertices.push_back(x * m_baseRadius);
        vertices.push_back(-m_height / 2.0f);
        vertices.push_back(z * m_baseRadius);
        // Normal
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);

        // Position (top)
        vertices.push_back(x * m_topRadius);
        vertices.push_back(m_height / 2.0f);
        vertices.push_back(z * m_topRadius);
        // Normal
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
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
    unsigned int baseCenterIndex = static_cast<unsigned int>(vertices.size() / 6);
    vertices.push_back(0.0f);
    vertices.push_back(-m_height / 2.0f);
    vertices.push_back(0.0f);
    // normal
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);
    vertices.push_back(0.0f);

    // Top center vertex
    unsigned int topCenterIndex = static_cast<unsigned int>(vertices.size() / 6);
    vertices.push_back(0.0f);
    vertices.push_back(m_height / 2.0f);
    vertices.push_back(0.0f);
    // Normal
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);

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

    SetupMesh();
}
void Mesh::CreateCapsule(const float& radius, const float& height) {
    vertices.clear();
    indices.clear();

    constexpr int SLICES = 32;
    constexpr int STACKS = 16;
    const float halfHeight = height * 0.5f;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;

    auto addVertex = [&](float x, float y, float z, float nx, float ny, float nz) {
        positions.emplace_back(x, y, z);
        normals.emplace_back(nx, ny, nz);
    };

    // Top hemisphere
    for (int stack = 0; stack <= STACKS / 2; ++stack) {
        float phi = glm::pi<float>() * stack / (STACKS / 2);
        float y = radius * cos(phi);
        float r = radius * sin(phi);

        for (int slice = 0; slice <= SLICES; ++slice) {
            float theta = glm::two_pi<float>() * slice / SLICES;
            float x = r * cos(theta);
            float z = r * sin(theta);
            addVertex(x, y + halfHeight, z, x / radius, y / radius, z / radius);
        }
    }

    // Cylinder
    for (int i = 0; i <= 1; ++i) {
        float y = halfHeight * (1 - 2 * i);
        for (int slice = 0; slice <= SLICES; ++slice) {
            float theta = glm::two_pi<float>() * slice / SLICES;
            float x = radius * cos(theta);
            float z = radius * sin(theta);
            addVertex(x, y, z, x / radius, 0.0f, z / radius);
        }
    }

    // Bottom hemisphere
    for (int stack = 0; stack <= STACKS / 2; ++stack) {
        float phi = glm::pi<float>() * stack / (STACKS / 2);
        float y = radius * cos(phi);
        float r = radius * sin(phi);

        for (int slice = 0; slice <= SLICES; ++slice) {
            float theta = glm::two_pi<float>() * slice / SLICES;
            float x = r * cos(theta);
            float z = r * sin(theta);
            addVertex(x, -y - halfHeight, z, x / radius, -y / radius, z / radius);
        }
    }

    // Indices for top hemisphere
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

    // Indices for cylinder
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

    // Indices for bottom hemisphere
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

    // Flatten the vertices and normals
    vertices.reserve(positions.size() * 6);
    for (size_t i = 0; i < positions.size(); ++i) {
        vertices.push_back(positions[i].x);
        vertices.push_back(positions[i].y);
        vertices.push_back(positions[i].z);
        vertices.push_back(normals[i].x);
        vertices.push_back(normals[i].y);
        vertices.push_back(normals[i].z);
    }

    SetupMesh();

    std::cout << "[Capsule] Vertices: " << positions.size()
            << ", Triangles: " << indices.size()/3 << "\n";

}

Mesh Mesh::CreateCube()
{
    Mesh cube("Cube");

    cube.vertices.reserve(24 * 6); // each of the 24 vertices has 6 floats (pos+normal)
    cube.indices.reserve(36);

    auto addVertex = [&](float px, float py, float pz, float nx, float ny, float nz)
    {
        cube.vertices.push_back(px);
        cube.vertices.push_back(py);
        cube.vertices.push_back(pz);
        cube.vertices.push_back(nx);
        cube.vertices.push_back(ny);
        cube.vertices.push_back(nz);
    };

    auto addFace = [&](float x0, float y0, float z0,
                       float x1, float y1, float z1,
                       float x2, float y2, float z2,
                       float x3, float y3, float z3,
                       float nx, float ny, float nz)
    {
        int startIndex = static_cast<int>( cube.vertices.size() / 6 );

        addVertex(x0,y0,z0, nx,ny,nz);
        addVertex(x1,y1,z1, nx,ny,nz);
        addVertex(x2,y2,z2, nx,ny,nz);
        addVertex(x3,y3,z3, nx,ny,nz);

        cube.indices.push_back(startIndex + 0);
        cube.indices.push_back(startIndex + 1);
        cube.indices.push_back(startIndex + 2);

        cube.indices.push_back(startIndex + 2);
        cube.indices.push_back(startIndex + 3);
        cube.indices.push_back(startIndex + 0);
    };

    // +Z face (front)
    addFace(
        -0.5f, -0.5f, +0.5f,
         0.5f, -0.5f, +0.5f,
         0.5f,  0.5f, +0.5f,
        -0.5f,  0.5f, +0.5f,
         0.0f,  0.0f, +1.0f
    );
    // -Z face (back)
    addFace(
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.0f,  0.0f, -1.0f
    );
    // +X face (right)
    addFace(
        +0.5f, -0.5f, +0.5f,
        +0.5f, -0.5f, -0.5f,
        +0.5f,  0.5f, -0.5f,
        +0.5f,  0.5f, +0.5f,
        +1.0f,  0.0f,  0.0f
    );
    // -X face (left)
    addFace(
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, +0.5f,
        -0.5f,  0.5f, +0.5f,
        -0.5f,  0.5f, -0.5f,
        -1.0f,  0.0f,  0.0f
    );
    // +Y face (top)
    addFace(
        -0.5f, +0.5f, +0.5f,
         0.5f, +0.5f, +0.5f,
         0.5f, +0.5f, -0.5f,
        -0.5f, +0.5f, -0.5f,
         0.0f, +1.0f,  0.0f
    );
    // -Y face (bottom)
    addFace(
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, +0.5f,
        -0.5f, -0.5f, +0.5f,
         0.0f, -1.0f,  0.0f
    );

    cube.SetupMesh();

    std::cout << "[Cube] vertices: " << (cube.vertices.size()/6)
              << ", indices: " << cube.indices.size()
              << ", triangles: " << (cube.indices.size()/3)
              << std::endl;

    return cube;
}