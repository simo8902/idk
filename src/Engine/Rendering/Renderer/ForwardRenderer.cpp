//
// Created by SIMEON on 2/22/2025.
//

#include "ForwardRenderer.h"

#include "Collider.h"
#include "MeshRenderer.h"

ForwardRenderer::ForwardRenderer(const std::shared_ptr<IDK::Scene>& scene, const std::shared_ptr<IDK::Graphics::Camera>& camera,
                                 GLFWwindow* window, const std::string& rendererType)
    : scene(scene), camera(camera), window(window), rendererType(rendererType),
      FBO(0), texture_id(0), RBO(0), width(0), height(0)
{
    std::cerr << "FORWARD_RENDERER()" << std::endl;

    if (!scene || !camera || !window) {
        throw std::invalid_argument("One or more parameters are null");
    }
    scene->createObjects();
}

ForwardRenderer::~ForwardRenderer() {
    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
    }
    if (texture_id) {
        glDeleteTextures(1, &texture_id);
    }
}
void ForwardRenderer::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardRenderer::updateFramebuffer(int viewportWidth, int viewportHeight) {
    if (viewportWidth == this->width && viewportHeight == this->height)
        return;

    this->width = viewportWidth;
    this->height = viewportHeight;

    if (FBO != 0) {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }
    if (texture_id != 0) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
    if (RBO != 0) {
        glDeleteRenderbuffers(1, &RBO);
        RBO = 0;
    }

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, viewportWidth, viewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << "ForwardRenderer::framebuffer updated." << std::endl;
}

void ForwardRenderer::resizeFramebuffer(int width, int height) {
    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
    }
    if (texture_id) {
        glDeleteTextures(1, &texture_id);
    }
    if (RBO) {
        glDeleteRenderbuffers(1, &RBO);
    }
}