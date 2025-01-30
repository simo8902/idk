//
// Created by Simeon on 4/5/2024.
//

#include "Scene.h"
#include "Cube.h"
#include "Capsule.h"
#include "Sphere.h"
#include "Cylinder.h"

std::vector<std::shared_ptr<GameObject>> Scene::objects;
std::vector<std::shared_ptr<Component>> Scene::components;

Scene::Scene(const std::shared_ptr<Shader> & shaderProgram, const std::shared_ptr<Shader> & lightingShader, const std::shared_ptr<Shader> & finalPassShader,
    const std::shared_ptr<Camera> & camera, const std::shared_ptr<LightManager> & lightManager)
    : gPosition(0), gAlbedoSpec(0),
    gNormal(0),
    gBuffer(0), lightingShader(lightingShader), lightingTexture(0), finalPassTexture(0),
    lightingFramebuffer(0),
    rboDepth(0),
    width(0), height(0), skyboxTexture(0), skyVAO(0),
    skyVBO(0),
    shaderProgram(shaderProgram),
    finalPassShader(finalPassShader),
    m_Camera(camera),
    lightManager(lightManager)
{
    initFullscreenQuad();

    if (!shaderProgram || !lightingShader  || !finalPassShader || !camera || !lightManager)
    {
        std::cerr << "[Scene] One or more parameters are null!" << std::endl;
        throw std::runtime_error("Scene ctor received null parameters");
    }
    createObjects();
}

Scene::~Scene() {
    objects.clear();

    lightingShader = nullptr;
    shaderProgram = nullptr;
    finalPassShader = nullptr;

    if (gBuffer) glDeleteFramebuffers(1, &gBuffer);
    if (lightingFramebuffer) glDeleteFramebuffers(1, &lightingFramebuffer);
    if (skyVAO) glDeleteVertexArrays(1, &skyVAO);
    if (skyVBO) glDeleteBuffers(1, &skyVBO);
    if (gPosition) glDeleteTextures(1, &gPosition);
    if (gNormal) glDeleteTextures(1, &gNormal);
    if (gAlbedoSpec) glDeleteTextures(1, &gAlbedoSpec);
    if (lightingTexture) glDeleteTextures(1, &lightingTexture);
    if (rboDepth) glDeleteRenderbuffers(1, &rboDepth);
    if (skyboxTexture) glDeleteTextures(1, &skyboxTexture);
    if (finalPassTexture) glDeleteTextures(1, &finalPassTexture);
    if (finalFramebuffer) glDeleteFramebuffers(1, &finalFramebuffer);
}

void Scene::initFullscreenQuad()
{
    if (quadVAO != 0)
        return;

    float quadVertices[] = {
        // positions (2D)  // texcoords (flipped V)
        -1.0f,  1.0f,    0.0f, 0.0f,  // bottom left
        -1.0f, -1.0f,    0.0f, 1.0f,  // top left
         1.0f, -1.0f,    1.0f, 1.0f,  // top right
        -1.0f,  1.0f,    0.0f, 0.0f,  // bottom left
         1.0f, -1.0f,    1.0f, 1.0f,  // top right
         1.0f,  1.0f,    1.0f, 0.0f   // bottom right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // pos attribute (vec2)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // texcoord attribute (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}
void Scene::RenderGeometryPass() const
{
    if (!shaderProgram || !m_Camera) return;

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    const auto& viewF = glm::mat4(m_Camera->getViewMatrix());
    const auto& projectF = glm::mat4(m_Camera->getProjectionMatrix());

    shaderProgram->Use();
    shaderProgram->setMat4("view", viewF);
    shaderProgram->setMat4("projection", projectF);

    for (const auto& obj : components) {
        const auto transform = obj->getComponent<Transform>();
        if (!transform) {
            std::cerr << "[ERROR] Missing Transform for object: " << "\n";
            continue;
        }

        glm::mat4 model = transform->getModelMatrix();
        shaderProgram->setMat4("model", model);

        auto collider = obj->getComponent<Collider>();
        if (collider) {
            const GLuint & programID = shaderProgram->getProgramID();

            shaderProgram->Use();
            shaderProgram->setMat4("view", viewF);
            shaderProgram->setMat4("projection", projectF);
            shaderProgram->setMat4("model", model);

            GLint wireframeLoc = glGetUniformLocation(programID, "wireframe");
            GLint wireframeColorLoc = glGetUniformLocation(programID, "wireframeColor");

            if (wireframeLoc == -1 || wireframeColorLoc == -1) {
                std::cerr << "[ERROR] Uniform 'wireframe' or 'wireframeColor' not found in shader program.\n";
            } else {
                glUniform1i(wireframeLoc, GL_TRUE);
                glUniform3f(wireframeColorLoc, 0.0f, 1.0f, 0.0f);
            }
            collider->Draw(*shaderProgram);
            glUniform1i(wireframeLoc, GL_FALSE);

        }


        if (const auto & meshRenderer = obj->getComponent<MeshRenderer>())
        {
            shaderProgram->Use();
            shaderProgram->setMat4("view", viewF);
            shaderProgram->setMat4("projection", projectF);
            shaderProgram->setMat4("model", model);
            shaderProgram->setVec3("objectColor", glm::vec3(0.2f, 0.2f, 0.2f));

            meshRenderer->Render(shaderProgram.get());
        }
    }

     DrawGrid(10.0f, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::RenderLightingPass() const
{
    lightingShader->Use();

    glBindFramebuffer(GL_FRAMEBUFFER, lightingFramebuffer);
  //  std::cout << "[Scene] Bound to lightingFramebuffer ID: " << lightingFramebuffer << "\n";

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind G-buffer textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    lightingShader->setInt("gPosition", 0);
  //  std::cout << "[Scene] Bound gPosition texture ID: " << gPosition << " to GL_TEXTURE0.\n";

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    lightingShader->setInt("gNormal", 1);
  //  std::cout << "[Scene] Bound gNormal texture ID: " << gNormal << " to GL_TEXTURE1.\n";

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    lightingShader->setInt("gAlbedoSpec", 2);
  //  std::cout << "[Scene] Bound gAlbedoSpec texture ID: " << gAlbedoSpec << " to GL_TEXTURE2.\n";

   // lightingShader->setVec3("viewPos", m_Camera->getPosition());

    static bool isMessagePrinted = false; // Static variable to track if the message was printed

    auto dirLights = lightManager->getDirectionalLights();
    bool test = false;

    for (const auto& dirLight : dirLights) {
        if (dirLight) {
            dirLight->setUniforms(lightingShader->getProgramID());
            test = true;
        }
    }

    if (test && !isMessagePrinted) {
        std::cout << "[Scene] Number of directional lights: " << dirLights.size() << "\n";
        isMessagePrinted = true;
    }

    glEnable(GL_BLEND);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Scene::RenderFinalPass() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    finalPassShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    finalPassShader->setInt("gPosition", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    finalPassShader->setInt("gNormal", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    finalPassShader->setInt("gAlbedoSpec", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, lightingTexture);
    finalPassShader->setInt("lightingTexture", 3);

    // Render a full-screen quad to combine the data
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::initFinalFramebuffer() {
    glGenFramebuffers(1, &finalFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, finalFramebuffer);

    glGenTextures(1, &finalPassTexture);
    glBindTexture(GL_TEXTURE_2D, finalPassTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalPassTexture, 0);

    checkFramebufferStatus();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Scene::createObjects() {
    // -----------------------------------------------------------------------------
    // CAPSULE
    // -----------------------------------------------------------------------------
    const auto& capsule = std::make_shared<Capsule>("Capsule");
    capsule->addComponents();
    components.emplace_back(capsule);

    // -----------------------------------------------------------------------------
    // CUBE
    // -----------------------------------------------------------------------------
    const auto& cube = std::make_shared<Cube>("Cube");
    cube->addComponents();
    components.emplace_back(cube);

    // -----------------------------------------------------------------------------
    // CYLINDER
    // -----------------------------------------------------------------------------
    const auto& cylinder = std::make_shared<Cylinder>("Cylinder");
    cylinder->addComponents();
    components.emplace_back(cylinder);

    // -----------------------------------------------------------------------------
    // SPHERE
    // -----------------------------------------------------------------------------
    const auto& sphere = std::make_shared<Sphere>("Sphere");
    sphere->addComponents();
    components.emplace_back(sphere);

}

void Scene::renderSky() const {
/*
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
    glDepthFunc(GL_LESS);*/

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) nullptr);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(nullptr));

    glDrawArrays(GL_LINES, 0, gridVertices.size());

    glDisableVertexAttribArray(0);

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

void Scene::updateViewportFramebuffer(int viewportWidth, int viewportHeight) {
    if (viewportWidth == this->width && viewportHeight == this->height)
        return;

    this->width = viewportWidth;
    this->height = viewportHeight;

    this->width = viewportWidth;
    this->height = viewportHeight;

    if (gBuffer != 0) {
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedoSpec);
        glDeleteRenderbuffers(1, &rboDepth);
    }

    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, width, height); // Immutable storage
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Faster filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, width, height); // Immutable storage
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Faster filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height); // Immutable storage
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Faster filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    constexpr GLuint attachments[3] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    checkFramebufferStatus();

    if (lightingFramebuffer != 0) {
        glDeleteFramebuffers(1, &lightingFramebuffer);
        glDeleteTextures(1, &lightingTexture);
    }

    glGenFramebuffers(1, &lightingFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, lightingFramebuffer);

    glGenTextures(1, &lightingTexture);
    glBindTexture(GL_TEXTURE_2D, lightingTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height); // Immutable storage
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Faster filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightingTexture, 0);

    constexpr GLuint lightingAttachments[1] = {
        GL_COLOR_ATTACHMENT0
    };
    glDrawBuffers(1, lightingAttachments);

    if (const GLenum &status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[ERROR] Lighting Framebuffer is not complete! Status = 0x" << std::hex << status << std::endl;
    }
    else {
      //  std::cout << "[INFO] Lighting Framebuffer is complete.\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Scene::getLightingTexture() const {
  //  std::cout << "[INFO] getLightingTexture() called. lightingTexture ID: " << lightingTexture << "\n";
    return lightingTexture;
}

void Scene::checkFramebufferStatus()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[Scene] Framebuffer not complete! Status: ";
        switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            std::cerr << "GL_FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;
        default:
            std::cerr << "Unknown Error";
        }
        std::cerr << std::endl;
    } else {
       // std::cout << "[Scene] Framebuffer is complete." << std::endl;
    }
}