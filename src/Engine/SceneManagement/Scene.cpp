//
// Created by Simeon on 4/5/2024.
//

#include "Scene.h"

#include "AssetManager.h"
#include "Cube.h"
#include "Capsule.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Entity.h"
#include "SceneManager.h"
#include "ShaderManager.h"

Scene::Scene(const std::shared_ptr<Camera> & camera): skyboxTexture(0), skyVAO(0), skyVBO(0) {
    std::cerr << "SCENE()" << std::endl;

    lightManager = std::make_shared<LightManager>();

    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    const auto &directionalLight = std::make_shared<DirectionalLight>(
        "Main Light",
        lightDirection,
        glm::vec3(0.9f, 0.9f, 0.9f), // Ambient
        glm::vec3(1.0f, 1.0f, 1.0f), // Diffuse
        glm::vec3(1.0f, 1.0f, 1.0f) // Specular
    );
    directionalLight->updateDirectionFromRotation();

    lightManager->addDirectionalLight(directionalLight);

    // this->skyboxTexture = skyboxTexture;
    // std::cerr << "Scene::CTOR" << std::endl;

    /*
    if (!camera || !lightManager) {
        std::cerr << "[Scene] One or more parameters are null!" << std::endl;
        throw std::runtime_error("Scene ctor received null parameters");
    }*/

    // setupSky();
}

Scene::~Scene() {
    for (const auto& comp : components) {
        if (!comp) {
            std::cerr << "[ERROR] nullptr component before clearing" << std::endl;
        }
    }

    components.clear();

    if (skyVAO) glDeleteVertexArrays(1, &skyVAO);
    if (skyVBO) glDeleteBuffers(1, &skyVBO);
    if (skyboxTexture) glDeleteTextures(1, &skyboxTexture);
}

const std::vector<std::shared_ptr<Entity>> & Scene::getComponents() const {
    return components;
}

void Scene::createObjects() {
    const auto& rootFolder = AssetManager::getInstance().getRootFolder();

    // -----------------------------------------------------------------------------
    // CAPSULE
    // -----------------------------------------------------------------------------
    const auto&  capsule = std::make_shared<Capsule>("Capsule");
    capsule->addComponents();
    const auto& capsuleEntity = std::make_shared<Entity>(capsule);
    components.emplace_back(capsuleEntity);

    const auto& capsuleAsset = CreateGameObjectAssetItem(capsule);
    SceneManager::getInstance().addObject(capsule);

    /*
    std::cerr << "Before getInstance()" << std::endl;
    auto& assetManager = AssetManager::getInstance();
    std::cerr << "After getInstance()" << std::endl;

    std::cerr << "Before getRootFolder()" << std::endl;
    auto rootFolder = AssetManager::getInstance().getRootFolder();
    std::cerr << "After getRootFolder(): " << rootFolder << std::endl;*/

    /*
    auto sceneFolder  = AssetManager::getInstance().getRootFolder()->getChildByName("Scene");
    std::cerr << "Scene Folder: " << sceneFolder->getName()  << std::endl;
    if (sceneFolder) {
        sceneFolder->addChild(capsuleAsset);
        std::cerr << "Added GameObject to Scene folder: " << capsuleAsset->getName() << std::endl;
    }else if (!sceneFolder) {
        std::cerr << "Scene folder not found!" << std::endl;
        return;
    }*/

    if (rootFolder) {
        rootFolder->addChild(capsuleAsset);
        std::cerr << "Added GameObject directly to ROOT: " << capsuleAsset->getName() << std::endl;
    } else {
        std::cerr << "Root folder not found!" << std::endl;
        return;
    }


    // -----------------------------------------------------------------------------
    // CUBE
    // -----------------------------------------------------------------------------
    // AssetManager::getInstance().registerVirtualAsset(cube);

    const auto& cube = std::make_shared<Cube>("Cube");
    cube->Initialize();
    const auto&  cubeEntity = std::make_shared<Entity>(cube);
    components.emplace_back(cubeEntity);

    const auto &cubeAsset = CreateGameObjectAssetItem(cube);
    SceneManager::getInstance().addObject(cube);

    if (rootFolder) {
        rootFolder->addChild(cubeAsset);
        std::cerr << "Added GameObject directly to ROOT: " << cubeAsset->getName() << std::endl;
    } else {
        std::cerr << "Root folder not found!" << std::endl;
        return;
    }
   
    /*
    auto sceneFolder = AssetManager::getInstance().getRootFolder()->getChildByName("Scene");
    std::cerr << "asd: " <<sceneFolder << std::endl;
    if (sceneFolder) {
        sceneFolder->addChild(cubeAsset);
        std::cerr << "Added GameObject to Scene folder: " << cube->getName() << std::endl;
    }*/



   // auto cubeEntity = std::make_shared<Entity>(cube);
   // components.emplace_back(cubeEntity);

    /*
    if (std::shared_ptr<AssetItem> asset = std::dynamic_pointer_cast<AssetItem>(cube))
    {
        std::cout << "Cube is an AssetItem!" << std::endl;
        std::cout << "Name: " << asset->getName() << ", Type: " << static_cast<int>(asset->getType()) << std::endl;
    } else {
        std::cout << "Cube is NOT an AssetItem!" << std::endl;
    }*/

    // -----------------------------------------------------------------------------
    // CYLINDER
    // -----------------------------------------------------------------------------

    const auto&  cylinder = std::make_shared<Cylinder>("Cylinder");
    cylinder->addComponents();

    const auto&  cylinderEntity = std::make_shared<Entity>(cylinder);
    components.emplace_back(cylinderEntity);
    const auto &cylinderAsset = CreateGameObjectAssetItem(cylinder);
    SceneManager::getInstance().addObject(cylinder);

    if (rootFolder) {
        rootFolder->addChild(cylinderAsset);
    }

    // -----------------------------------------------------------------------------
    // SPHERE
    // -----------------------------------------------------------------------------

    const auto& sphere = std::make_shared<Sphere>("Sphere");
    sphere->addComponents();

    const auto& sphereEntity = std::make_shared<Entity>(sphere);
    components.emplace_back(sphereEntity);

   // const auto &sphereAsset = CreateGameObjectAssetItem(sphere);
   // SceneManager::getInstance().addObject(sphere);

    auto sphereEntAsset = std::make_shared<AssetItem>(sphere->getName(), AssetType::Entity, "", true);
    SceneManager::getInstance().addObject(sphere);
    SceneManager::getInstance().addObject(sphereEntity);

    sphereEntAsset->setEntityObject(sphereEntity);

    if (rootFolder) {
        rootFolder->addChild(sphereEntAsset);
    }
}

void Scene::renderSky() {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    auto skyShader = ShaderManager::Instance().getSkyShader();
    skyShader->Use();

    glm::mat4 view = glm::mat4(glm::mat3(m_Camera->getViewMatrix()));
    glm::mat4 projection = m_Camera->getProjectionMatrix();

    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f));
    skyShader->setMat4("view", view);
    skyShader->setMat4("projection", projection);
    skyShader->setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    skyShader->setInt("skybox", 0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindVertexArray(skyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
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

    const auto & shaderProgram = ShaderManager::Instance().getShaderProgram();

    shaderProgram->Use();
    shaderProgram->setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 model = gridTransform.getModelMatrix();
    shaderProgram->setMat4("model", model);

    GLuint gridVAO;
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);

    GLuint gridVBO;
    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(Vertex), gridVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) nullptr);

    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertices.size());

    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
}


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
