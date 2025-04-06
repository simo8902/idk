//
// Created by SIMEON on 2/22/2025.
//

#include "DeferredRenderer.h"

#include "Collider.h"
#include "MeshRenderer.h"

DeferredRenderer::DeferredRenderer(const std::shared_ptr<IDK::Scene>& scene, const std::shared_ptr<IDK::Graphics::Camera>& camera,
                                   GLFWwindow* window, const std::string& rendererType)
    : gPosition(0),
        gNormal(0),
        gAlbedoSpec(0),
        shaderProgram(ShaderManager::Instance().getShaderProgram()),
        lightingShader(ShaderManager::Instance().getLightShader()),
        finalPassShader(ShaderManager::Instance().getFinalPassShader()), window(window),
        rendererType(rendererType), gBuffer(0),
        rboDepth(0),
        lightingFramebuffer(0), lightingTexture(0),
        finalFramebuffer(0), finalPassTexture(0),
        quadVAO(0), quadVBO(0),
        width(0),
        height(0),
        scene(scene),
        camera(camera)
{
   // std::cerr << "DeferredRenderer::CTOR init()" << std::endl;
    initLightManager();
    initFullscreenQuad();

    scene->createObjects();

}
void DeferredRenderer::initLightManager() {
    lightManager = scene->getLightManager();
}
DeferredRenderer::~DeferredRenderer() {
    shaderProgram = nullptr;
    lightingShader = nullptr;
    finalPassShader = nullptr;
    window = nullptr;

    if (gBuffer) glDeleteFramebuffers(1, &gBuffer);
    if (lightingFramebuffer) glDeleteFramebuffers(1, &lightingFramebuffer);
    if (finalFramebuffer) glDeleteFramebuffers(1, &finalFramebuffer);
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
    if (gPosition) glDeleteTextures(1, &gPosition);
    if (gNormal) glDeleteTextures(1, &gNormal);
    if (gAlbedoSpec) glDeleteTextures(1, &gAlbedoSpec);
    if (lightingTexture) glDeleteTextures(1, &lightingTexture);
    if (finalPassTexture) glDeleteTextures(1, &finalPassTexture);
    if (rboDepth) glDeleteRenderbuffers(1, &rboDepth);
}

void DeferredRenderer::render() {
    RenderGeometryPass();
    RenderLightingPass();
    RenderFinalPass();
   // std::cerr << "DeferredRenderer Render Complete" << std::endl;
}

void DeferredRenderer::initFullscreenQuad() {
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

void DeferredRenderer::updateViewportFramebuffer(const int & viewportWidth,const int & viewportHeight) {
    if (viewportWidth == this->width && viewportHeight == this->height)
        return;

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

    std::cout << "DeferredRenderer::framebuffer updated." << std::endl;
}

void DeferredRenderer::RenderGeometryPass()const {

   if (!shaderProgram || !camera) return;

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    const auto& viewF = glm::mat4(camera->getViewMatrix());
    const auto& projectF = glm::mat4(camera->getProjectionMatrix());

    shaderProgram->Use();
    shaderProgram->setMat4("view", viewF);
    shaderProgram->setMat4("projection", projectF);

    for (const auto& obj : scene->getComponents()) {

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

        if (const auto & meshRenderer = obj->getComponent<IDK::Components::MeshRenderer>())
        {
            shaderProgram->Use();
            shaderProgram->setMat4("view", viewF);
            shaderProgram->setMat4("projection", projectF);
            shaderProgram->setMat4("model", model);
            shaderProgram->setVec3("objectColor", glm::vec3(0.2f, 0.2f, 0.2f));

            meshRenderer->Render(shaderProgram.get());
        }
    }
    scene->DrawGrid(10.0, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::RenderLightingPass() const{
    lightingShader->Use();

    if (lightingShader == nullptr && camera == nullptr && lightManager == nullptr) {
        std::cerr << "DeferredRenderer::RenderLightingPass One of the components are null\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, lightingFramebuffer);
    //  std::cout << "[DefRenderer] Bound to lightingFramebuffer ID: " << lightingFramebuffer << "\n";

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (gPosition == 0 || gNormal == 0 || gAlbedoSpec == 0) {
        std::cerr << "RenderLightingPass ERROR: One or more G-Buffer textures are invalid!" << std::endl;
        return;
    }

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

     lightingShader->setVec3("viewPos", camera->getPosition());

    static bool isMessagePrinted = false;

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

  //  std::cout << "[DefRenderer] Lighting pass completed.\n";
}

void DeferredRenderer::RenderFinalPass() const {
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

void DeferredRenderer::checkFramebufferStatus() {
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

void DeferredRenderer::resizeFramebuffer(int newWidth, int newHeight) {
    glDeleteFramebuffers(1, &gBuffer);
    glDeleteFramebuffers(1, &lightingFramebuffer);
    glDeleteFramebuffers(1, &finalFramebuffer);
    glDeleteTextures(1, &gPosition);
    glDeleteTextures(1, &gNormal);
    glDeleteTextures(1, &gAlbedoSpec);
    glDeleteTextures(1, &lightingTexture);
    glDeleteTextures(1, &finalPassTexture);
    glDeleteRenderbuffers(1, &rboDepth);

    width = newWidth;
    height = newHeight;
}
