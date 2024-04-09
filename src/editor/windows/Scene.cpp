//
// Created by Simeon on 4/5/2024.
//

#include "Scene.h"
#include "imgui.h"
#include "../../components/Transform.h"
#include "../../Utils.h"
#include "../../components/colliders/BoxCollider.h"

using Utils::operator<<;

Scene::Scene(Shader* shader) : m_shader(shader), FBO_width(0), FBO_height(0) {
    fov = glm::radians(90.0f);
    gridTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
}

Scene::~Scene() {

}

std::ostream& operator<<(std::ostream& out, const glm::mat4& mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            out << mat[i][j] << " ";
        }
        out << std::endl;
    }
    return out;
}


void Scene::update() {
    // Currently a placeholder - will be replaced with actual updates later
    /*
    static float rotationAngle = 0.0f;
    cube->setRotation(glm::vec3(0.0f, rotationAngle, 0.0f));
    rotationAngle += 0.5f;*/
}


void Scene::Render3DScene(float display_w, float display_h) {
     glViewport(0, 0, static_cast<int>(display_w), static_cast<int>(display_h));

     if (m_shader == nullptr) {
         std::cout << "m_shader is not initialized!" << std::endl;
         return;
     }

    m_shader->Use();

    glm::mat4 view = calculateViewMatrix();
    glm::mat4 projection = calculateProjectionMatrix(display_w, display_h);

    m_shader->setMat4("view", view);
    m_shader->setMat4("projection", projection);

    for (const std::shared_ptr<GameObject>& obj  : m_objects) {
        // std::cout << "Rendering object: " << obj->getName() << std::endl;

        Transform *transformComponent = obj->getComponent<Transform>();
        BoxCollider *boxCollider = obj->getComponent<BoxCollider>();

        if (transformComponent) {
           // std::cout << "Address of Transform component: " <<* transformComponent << std::endl;

            glm::mat4 model = transformComponent->getModelMatrix();
            m_shader->setMat4("model", model);
             m_shader->setVec3("objectColor", obj->getColor());

            // std::cout << "Model matrix: " << model << std::endl;

        }

        if (boxCollider != nullptr) {
            obj->DebugDraw(*m_shader);
         }

        obj->Draw(*m_shader);
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glFlush();
}

void Scene::renderSceneView(int display_w, int display_h) {
    glViewport(0, 0, display_w, display_h);
     ImGui::Begin("Scene View");

    // Only resize the fbo if the display size has actually changed
    if (FBO_width != display_w || FBO_height != display_h) {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture_id);
        create_framebuffer(display_w, display_h);
        FBO_width = display_w;
        FBO_height = display_h;
    }

    if (!depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
        depthTestEnabled = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawGrid(10.0f, 1.0f);

    Render3DScene(display_w, display_h);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, display_w, display_h); // Restore viewport

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::Image((void*)(intptr_t)texture_id, windowSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

void Scene::create_framebuffer(float display_w, float display_h) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_w, display_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    // Check for errors after each OpenGL call
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << error << " - Failed to create color texture attachment." << std::endl;
        return;
    }

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, display_w, display_h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // Check for errors after each OpenGL call
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << error << " - Failed to create renderbuffer attachment." << std::endl;
        return;
    }

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

struct Vertex {
    glm::vec3 position;
};

void Scene::DrawGrid(float gridSize, float gridStep) {
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


    m_shader->Use();
    m_shader->setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 model = gridTransform.getModelMatrix();
    m_shader->setMat4("model", model);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glDrawArrays(GL_LINES, 0, gridVertices.size());

    glDisableVertexAttribArray(0);

    // Delete the VBO if you won't need the grid again
    glDeleteBuffers(1, &gridVBO);
}

glm::mat4 Scene::calculateViewMatrix() {
    glm::vec3 cameraPosition = glm::vec3(0.0f, 1.0f, -10.0f);
    glm::vec3 cameraTarget   = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp       = glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::lookAt(cameraPosition, cameraTarget, cameraUp);
}

glm::mat4 Scene::calculateProjectionMatrix(int display_w, int display_h) {
    float aspectRatio = (display_w > 0 && display_h > 0) ?
                        (float)display_w / (float)display_h : 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

void Scene::renderGameView(){
    ImGui::Begin("Game View");
    ImGui::Text("Game View");
    ImGui::End();
}
