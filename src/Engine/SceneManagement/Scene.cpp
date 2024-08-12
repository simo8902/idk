//
// Created by Simeon on 4/5/2024.
//

#include "Scene.h"
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

Scene::Scene(Shader* shaderProgram, Shader* wireframe, const std::shared_ptr<Camera> & camera):
    shaderProgram(shaderProgram), wireframe(wireframe), m_Camera(camera){}

Scene::~Scene(){}

void Scene::Render3DScene()const{
    const glm::mat4 & view = m_Camera->getViewMatrix();
    const glm::mat4 & projection = m_Camera->getProjectionMatrix();

    shaderProgram->Use();
    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);

    const auto & objectColor = glm::vec3(0.2f, 0.2f, 0.2f);
    shaderProgram->setVec3("objectColorUniform", objectColor);

    for (const auto& obj : objects) {
        const auto* transformComponent = obj->getComponent<Transform>();

        if (transformComponent == nullptr) {
            std::cerr << "Transform component is null\n";
            continue;
        }

        glm::mat4 model = transformComponent->getModelMatrix();
        shaderProgram->setMat4("model", model);

        if (auto* collider = obj->getComponent<Collider>()) {
            wireframe->Use();
            wireframe->setMat4("m_View", view);
            wireframe->setMat4("m_Projection", projection);

            glm::mat4 wireModel = transformComponent->getModelMatrix();
            wireframe->setMat4("m_Model", wireModel);

            if (auto* boxCollider = dynamic_cast<BoxCollider*>(collider)) {
                boxCollider->Draw(*wireframe);
            }
            else if (auto* capsuleCollider = dynamic_cast<CapsuleCollider*>(collider)) {
                capsuleCollider->Draw(*wireframe);
            }
            else if (auto* cylinderCollider = dynamic_cast<CylinderCollider*>(collider)) {
                cylinderCollider->Draw(*wireframe);
            }
            else if(auto* sphereCollider = dynamic_cast<SphereCollider*>(collider)) {
                sphereCollider->Draw(*wireframe);
            }
            else {
                std::cerr << "Unsupported collider type\n";
            }
        }

        obj->Draw(*shaderProgram);
    }
}

void Scene::createObjects() {
    // Capsule1
    const auto capsule1 = std::make_shared<Capsule>("Capsule1");
    capsule1->addComponent<Transform>();

    Transform* capsule1Transform = capsule1->getComponent<Transform>();
    float radius = 1.0f;
    float height = 2.0f;
    capsule1->addComponent<CapsuleCollider>(capsule1Transform->getPosition(), radius, height);

    // Cube1
    const auto cube1 = std::make_shared<Cube>("Cube1");
    cube1->addComponent<Transform>();
    Transform* cube1Transform = cube1->getComponent<Transform>();
    cube1Transform->setPosition(glm::vec3(4.2f, 1.5f, -2.5f));
    cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));

    // Cylinder
    float baseRadius = 0.5f;
    float topRadius = 0.5f;
    float cheight = 2.0f;
    int sectors = 30;

    const auto cylinder1 = std::make_shared<Cylinder>("Cylinder1", baseRadius, topRadius, cheight, sectors);
    cylinder1->addComponent<Transform>();
    Transform* cylinder1Transform = cylinder1->getComponent<Transform>();
    cylinder1Transform->setPosition(glm::vec3(2.20f, 1.65f, -2.50f));
    cylinder1->addComponent<CylinderCollider>(cylinder1Transform->getPosition(), cheight,baseRadius);



    // sphere
    const auto sphere1 = std::make_shared<Sphere>("Sphere1");
    sphere1->addComponent<Transform>();
    Transform* sphere1Transform = sphere1->getComponent<Transform>();
    sphere1Transform->setPosition(glm::vec3(-2.20f, 1.65f, -2.50f));
    sphere1->addComponent<SphereCollider>(sphere1Transform->getPosition(), 1.2f, glm::vec3(0.0f, 0.0f, 0.0f));

    objects.push_back(cube1);
    objects.push_back(capsule1);
    objects.push_back(cylinder1);
    objects.push_back(sphere1);

    for (auto obj : objects) {
        // obj->printComponents();
    }
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
    shaderProgram->setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f));

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
