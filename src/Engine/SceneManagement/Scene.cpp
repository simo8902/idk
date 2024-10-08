//
// Created by Simeon on 4/5/2024.
//

#include "Scene.h"

#include "AssetManager.h"
#include "Transform.h"
#include ".h/Collider.h"
#include ".h/CapsuleCollider.h"
#include ".h/CylinderCollider.h"
#include ".h/SphereCollider.h"
#include ".h/BoxCollider.h"
#include ".h/Cube.h"
#include ".h/Capsule.h"
#include ".h/Sphere.h"
#include ".h/Cylinder.h"

std::vector<std::shared_ptr<GameObject>> Scene::objects;
std::vector<std::shared_ptr<Light>> Scene::lights;

Scene::Scene(const std::shared_ptr<Shader> & shaderProgram,const std::shared_ptr<Shader> & wireframe, const std::shared_ptr<Shader> & sky,
    const std::shared_ptr<Camera> & camera, const GLuint & skyboxTexture):
    skyboxTexture(skyboxTexture), shaderProgram(shaderProgram) ,wireframe(wireframe),
    skyShader(sky), m_Camera(camera) {
    this->shaderProgram = shaderProgram;
    this->wireframe = wireframe;
    this->skyShader = sky;

    this->m_Camera = camera;
    this->skyboxTexture = skyboxTexture;

    createObjects();
    setupSky();

}

Scene::~Scene(){}

void Scene::setupSky() {
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);

    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Scene::renderSky() const {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    skyShader->Use();

    glm::mat4 view = glm::mat4(glm::mat3(m_Camera->getViewMatrix()));
    glm::mat4 projection = m_Camera->getProjectionMatrix();

    skyShader->setMat4("view", view);
    skyShader->setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    skyShader->setInt("skybox", 0);

    glBindVertexArray(skyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

}

void Scene::Render3DScene() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4& view = m_Camera->getViewMatrix();
    const glm::mat4& projection = m_Camera->getProjectionMatrix();
    renderSky();

    shaderProgram->Use();
    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);

    if (skyboxTexture == 0) {
        std::cerr << "Skybox texture failed to load." << std::endl;
    }

 //   const glm::vec3 objectColor = glm::vec3(0.2f, 0.2f, 0.2f);
 //   shaderProgram->setVec3("objectColorUniform", objectColor);

    glm::vec3 globalAmbientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram->getProgramID(), "globalAmbientColor"), 1, glm::value_ptr(globalAmbientColor));

    for (const auto& light : directionalLights) {
        light->setUniforms(shaderProgram->getProgramID());
    }

    for (const auto& obj : objects) {
        auto transform = obj->getComponent<Transform>();
        if (!transform) {
            std::cerr << "[ERROR] Missing Transform for object: " << obj->getName() << "\n";
            continue;
        }

        glm::mat4 model = transform->getModelMatrix();
        shaderProgram->setMat4("model", model);

        auto collider = obj->getComponent<Collider>();
        if (collider) {
            wireframe->Use();
            wireframe->setMat4("m_View", view);
            wireframe->setMat4("m_Projection", projection);
            wireframe->setMat4("m_Model", model);
            collider->Draw(*wireframe);
        }

        auto meshRenderer = obj->getComponent<MeshRenderer>();
        if (meshRenderer) {
            std::shared_ptr<Material> material = meshRenderer->getMaterial();
            if (material && material->getShader()) {
                auto shader = material->getShader();
                shader->Use();
                shader->setMat4("view", view);
                shader->setMat4("projection", projection);
                shader->setMat4("model", model);

                meshRenderer->Render(view, projection);
            } else {
                std::cerr << "[ERROR] Missing Material or Shader for object: " << obj->getName() << "\n";
            }
        } else {
            std::cerr << "[INFO] No MeshRenderer for object: " << obj->getName() << "\n";
        }
    }

    for (const auto& light : lights) {
        std::cerr << light->getName() << std::endl;
    }
}


void Scene::createObjects() {
    auto globalMaterial = std::make_shared<Material>("GlobalMaterial");
    AssetManager::getInstance().addMaterial(globalMaterial);
    globalMaterial->assignShader(shaderProgram);

    // Capsule1
    const auto capsule1 = std::make_shared<Capsule>("Capsule1");
    capsule1->addComponent<Transform>();
    auto capsule1Transform = capsule1->getComponent<Transform>();
    capsule1->addComponent<CapsuleCollider>(capsule1Transform->getPosition(), 1.0f, 2.0f);
    float resolution = 512.0f;

    auto capsuleMeshFilter = capsule1->addComponent<MeshFilter>(capsule1);

    auto capsuleMesh = std::make_shared<Mesh>("Capsule");
    capsuleMesh->CreateCapsule(1.0f, 2.0f, resolution);

    capsuleMeshFilter->setMesh(capsuleMesh);
    capsule1->addComponent<MeshRenderer>(capsule1, capsuleMeshFilter, globalMaterial);

    // Cube1
    const auto cube1 = std::make_shared<Cube>("Cube1");

    cube1->addComponent<Transform>();
    std::shared_ptr<Transform> cube1Transform = cube1->getComponent<Transform>();
    cube1Transform->setPosition(glm::vec3(4.2f, 1.5f, -2.5f));
    cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));

    auto meshFilter = cube1->addComponent<MeshFilter>(cube1);
    meshFilter->setMesh(std::make_shared<Mesh>(Mesh::CreateCube(), "Cube"));

    cube1->addComponent<MeshRenderer>(cube1, meshFilter, globalMaterial);

    // Cylinder1
    const auto cylinder1 = std::make_shared<Cylinder>("Cylinder1", 0.5f, 0.5f, 2.0f, 30);
    cylinder1->addComponent<Transform>();
    auto cylinder1Transform = cylinder1->getComponent<Transform>();
    cylinder1Transform->setPosition(glm::vec3(2.20f, 1.65f, -2.50f));
    cylinder1->addComponent<CylinderCollider>(cylinder1Transform->getPosition(), 2.0f, 0.5f);

    auto cylinderMeshFilter = cylinder1->addComponent<MeshFilter>(cylinder1);
    float baseRadius = 0.5f;
    float topRadius = 0.5f;
    float cheight = 2.0f;
    int sectors = 30;

    auto cylinderMesh = std::make_shared<Mesh>("Cylinder");
    cylinderMesh->CreateCylinder(baseRadius ,topRadius, cheight, sectors);
    cylinderMeshFilter->setMesh(cylinderMesh);

    cylinder1->addComponent<MeshRenderer>(cylinder1, cylinderMeshFilter, globalMaterial);

    // Sphere1
    const auto sphere1 = std::make_shared<Sphere>("Sphere1");
    sphere1->addComponent<Transform>();
    auto sphere1Transform = sphere1->getComponent<Transform>();
    sphere1Transform->setPosition(glm::vec3(-2.20f, 1.65f, -2.50f));
    sphere1->addComponent<SphereCollider>(sphere1Transform->getPosition(), 1.2f, glm::vec3(0.0f, 0.0f, 0.0f));

    auto sphereMeshFilter = sphere1->addComponent<MeshFilter>(sphere1);
    auto sphereMesh = std::make_shared<Mesh>("Sphere");
    sphereMesh->CreateSphere(1.0f, 1024, 32);

    sphereMeshFilter->setMesh(sphereMesh);
    sphere1->addComponent<MeshRenderer>(sphere1, sphereMeshFilter, globalMaterial);

    objects.push_back(capsule1);
    objects.push_back(cube1);
    objects.push_back(cylinder1);
    objects.push_back(sphere1);

    /*
    skySphere = std::make_shared<Sphere>("SkySphere");
    skySphere->addComponent<Transform>();
    Transform* skySphereTransform = skySphere->getComponent<Transform>();
    skySphereTransform->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));*/

    for (auto obj : objects) {
        // obj->printComponents();
    }
}

void Scene::createTemporalObject() {
    // Cube1
    const auto & cube1 = std::make_shared<Cube>("New Cube");
    cube1->addComponent<Transform>();
    std::shared_ptr<Transform> cube1Transform = cube1->getComponent<Transform>();
    cube1Transform->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));

    objects.push_back(cube1);
}

void Scene::DrawGrid(float gridSize, float gridStep) const {
    struct Vertex {
        glm::vec3 position;
    };

    std::vector<Vertex> gridVertices;

    for (float i = -gridSize; i <= gridSize; i += gridStep) {
        // Lines parallel to X-axis
        gridVertices.push_back({{i, 0.0f, -gridSize}});
        gridVertices.push_back({{i, 0.0f, gridSize}});

        // Lines parallel to Z-axis
        gridVertices.push_back({{-gridSize, 0.0f, i}});
        gridVertices.push_back({{gridSize, 0.0f, i}});
    }

    GLuint gridVBO;
    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(Vertex), gridVertices.data(), GL_STATIC_DRAW);

    shaderProgram->Use();
   // shaderProgram->setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 model = gridTransform.getModelMatrix();
    shaderProgram->setMat4("model", model);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    //  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) nullptr);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(nullptr));

    glDrawArrays(GL_LINES, 0, gridVertices.size());

    glDisableVertexAttribArray(0);

    glDeleteBuffers(1, &gridVBO);
}
