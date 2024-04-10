//
// Created by Simeon on 4/5/2024.
//

#include <sstream>
#include "Scene.h"
#include "imgui.h"
#include "../../components/Transform.h"
#include "../../Utils.h"
#include "../../components/colliders/BoxCollider.h"
#include <algorithm>

using Utils::operator<<;

Scene::Scene(Shader* shader) : m_shader(shader), FBO_width(0), FBO_height(0) {
    fov = glm::radians(90.0f);
    gridTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    m_gizmo = std::make_shared<Gizmo>();
}

Scene::~Scene() {

}

bool Scene::aabbIntersection(const Ray& ray, const BoxCollider* boxCollider) {
    glm::vec3 tmin = (boxCollider->getMin() - ray.getOrigin()) / ray.getDirection();
    glm::vec3 tmax = (boxCollider->getMax() - ray.getOrigin()) / ray.getDirection();

    // Find the largest minimum, smallest maximum in each dimension
    tmin = glm::min(tmin, tmax);
    tmax = glm::max(tmin, tmax);

    // Ensure the ray intersects in all 3 dimensions
    return (tmax.x >= tmin.x) && (tmax.y >= tmin.y) && (tmax.z >= tmin.z) &&
           (tmin.x <= 1.0f) && (tmin.y <= 1.0f) && (tmin.z <= 1.0f);
}


std::string vec3_to_string(const glm::vec3& v) {
    std::ostringstream oss;
    oss << "glm::vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
    return oss.str();
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

    for (const std::shared_ptr<GameObject>& obj : m_objects) {
        Transform *transformComponent = obj->getComponent<Transform>();
        BoxCollider *boxCollider = obj->getComponent<BoxCollider>();

        if (transformComponent) {
            glm::mat4 model = transformComponent->getModelMatrix();
            m_shader->setMat4("model", model);
        }

        if (boxCollider != nullptr) {
            obj->DebugDraw(*m_shader);
        }

        if (obj == m_selectedObject) {
            m_shader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f)); // Example: Yellow highlight
         //   m_gizmo->render(*m_shader, *obj->getComponent<Transform>());
        } else {
            m_shader->setVec3("objectColor", obj->getColor());
        }

        obj->Draw(*m_shader);
    }


    glClear(GL_DEPTH_BUFFER_BIT);
    glFlush();
}

void Scene::renderRay(const Ray& ray, const glm::vec3& color) {
    m_shader->Use();

    GLuint shaderProgramId = m_shader->getProgramId();
    glUseProgram(shaderProgramId);

    // Error checking for shader compilation and linking
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDisable(GL_LIGHTING);

    // Set line color
    glUniform3f(glGetUniformLocation(shaderProgramId, "objectColor"), color.x, color.y, color.z);

    // Switch to line mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Create Vertex Array Object (VAO) if not already created
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);

    // Bind vertex data (one vertex for origin, one for direction)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glm::vec3 vertices[2] = {
            ray.getOrigin(),
            ray.getOrigin() + ray.getDirection() * 10.0f // Extend ray by a visible amount
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Enable vertex attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Render the line
    glDrawArrays(GL_LINES, 0, 2);

    // Cleanup (optional)
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    // Cleanup (optional, if rendering multiple lines)
   // glDisableVertexAttribArray(0);
   // glBindVertexArray(0);

}

void Scene::renderSceneView(int display_w, int display_h) {
    glViewport(0, 0, display_w, display_h);
     ImGui::Begin("Scene View");

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) {
        ImVec2 mousePos = ImGui::GetMousePos();

        std::shared_ptr<GameObject> clickedObject = raycastFromMouse(mousePos, display_w, display_h);


        if (clickedObject != nullptr) {
            m_selectedObject = clickedObject;
            std::cout << "Clicked object: " << clickedObject->getName() << std::endl;
            inspectorManager->renderInspector(m_selectedObject);
        } else {
            m_selectedObject = nullptr; // Clear selection if clicked outside an object
            std::cout << "Clicked outside any object." << std::endl;
        }
    }

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

void Scene::renderGizmo(Transform* transform) {
    glm::mat4 modelMatrix = transform->getModelMatrix();

    // Set gizmo color
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow

    // Draw the wireframe cube
    glBegin(GL_LINES);
    // Bottom square
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);

    // Top square
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Connecting lines
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    glEnd();
}


std::shared_ptr<GameObject> Scene::raycastFromMouse(const ImVec2& mousePos, int display_w, int display_h) {
    std::shared_ptr<GameObject> clickedObject = nullptr;

    glm::vec2 normalizedMousePos = glm::vec2(mousePos.x / display_w, (display_h - mousePos.y) / display_h); // Flip y for NDC
    std::cout << "Normalized Mouse Position: (" << normalizedMousePos.x << ", " << normalizedMousePos.y << ")" << std::endl;

    glm::vec4 rayClip = glm::vec4(normalizedMousePos.x, normalizedMousePos.y, -1.0, 1.0);
    std::cout << "Ray Clip: (" << rayClip.x << ", " << rayClip.y << ", " << rayClip.z << ", " << rayClip.w << ")" << std::endl;

    // Ray Generation in Clip Space
    glm::mat4 projMatrix = calculateProjectionMatrix(display_w, display_h);
    std::cout << "Projection Matrix:" << std::endl;
    printMatrix(projMatrix);

    glm::vec4 rayEye = glm::inverse(projMatrix) * rayClip;
    std::cout << "Ray Eye: (" << rayEye.x << ", " << rayEye.y << ", " << rayEye.z << ", " << rayEye.w << ")" << std::endl;

    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0); // Setting z, w for direction

    // Ray Direction and Origin in World Space
    glm::mat4 viewMatrix = calculateViewMatrix();
    std::cout << "View Matrix:" << std::endl;
    printMatrix(viewMatrix);

    glm::vec4 rayWorld = glm::inverse(viewMatrix) * rayEye;
    std::cout << "Ray World: (" << rayWorld.x << ", " << rayWorld.y << ", " << rayWorld.z << ", " << rayWorld.w << ")" << std::endl;

    glm::vec3 cameraForward = glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);

    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));
    glm::vec3 rayOrigin = glm::vec3(rayWorld) - cameraForward * 0.1f; // Offset by 0.1 units

    std::cout << "Ray Origin: (" << rayOrigin.x << ", " << rayOrigin.y << ", " << rayOrigin.z << ")" << std::endl;
    std::cout << "Ray Direction: (" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << std::endl;

    // Unproject mouse position to get the ray origin in world space
    glm::vec3 nearPlaneWorld = glm::unProject(glm::vec3(mousePos.x, mousePos.y, 0.0f), viewMatrix, projMatrix, glm::vec4(0, 0, display_w, display_h));
    glm::vec3 farPlaneWorld = glm::unProject(glm::vec3(mousePos.x, mousePos.y, 1.0f), viewMatrix, projMatrix, glm::vec4(0, 0, display_w, display_h));

      glm::vec3 rayOriginWorld = glm::vec3(0.0f, 0.5f, -5.0f);
    glm::vec3 rayDirectionWorld = glm::vec3(0.0f, 0.0f, 1.0f); // Pointing directly forwards

    renderRay(Ray(rayOrigin, rayDirection), glm::vec3(1.0f, 0.0f, 0.0f)); // Red ray

    std::cout << "======= Raycasting Debug Info =======" << std::endl;
    std::cout << "Ray Origin (World Space): (" << rayOriginWorld.x << ", " << rayOriginWorld.y << ", " << rayOriginWorld.z << ")" << std::endl;
    std::cout << "Ray Direction (World Space): (" << rayDirectionWorld.x << ", " << rayDirectionWorld.y << ", " << rayDirectionWorld.z << ")" << std::endl;    std::cout << "-----------------------------------" << std::endl;

    clickedObject = nullptr;

    for (const std::shared_ptr<GameObject>& obj : m_objects) {
        BoxCollider* boxCollider = obj->getComponent<BoxCollider>();
        if (!boxCollider) {
            std::cerr << "Warning: Object " << obj->getName() << " does not have a BoxCollider component." << std::endl;
            continue;
        }
        std::cout << "=========================================================" << std::endl;
        Transform* cubeTransform = obj->getComponent<Transform>();
        std::cout << "Object Name: " << obj->getName() << std::endl;
        std::cout << "Model Matrix:" << std::endl;
        printMatrix(cubeTransform->getModelMatrix());
        std::cout << "=========================================================" << std::endl;

        std::cout << "BoxCollider Position: " << vec3_to_string(boxCollider->getPosition()) << std::endl;
        std::cout << "BoxCollider Min: " << vec3_to_string(boxCollider->getMin()) << std::endl;
        std::cout << "BoxCollider Max: " << vec3_to_string(boxCollider->getMax()) << std::endl;

        std::cout << "------------------------------------------- " << std::endl;


        if (boxCollider && aabbIntersection(Ray(rayOrigin, rayDirection), boxCollider)) {

            if(boxCollider->intersectsRay(Ray(rayOrigin, rayDirection))){

                clickedObject = obj;  // <-- Moved inside for correct logic
                std::cout << "Object clicked: " << obj->getName() << std::endl; // Print clicked object's name

                if (clickedObject != m_selectedObject) {
                    if (m_selectedObject) {
                        // Update objectColor for deselection
                        m_shader->Use();
                        m_shader->setVec3("objectColor", m_selectedObject->getColor());
                        m_selectedObject->Draw(*m_shader);
                    }
                    m_selectedObject = clickedObject;
                }
                break;
            }
        }
    }

    /*
    if (clickedObject != nullptr) {
        if (m_selectedObject && m_selectedObject != clickedObject) {
            m_selectedObject->setColor(m_selectedObject->getOriginalColor());
        }
        m_selectedObject = clickedObject;
        m_selectedObject->setColor(glm::vec3(1.0f, 1.0f, 0.0f)); // Highlight color
    } else */

    if (m_selectedObject) {
        std::cerr << "Deselection block executing!" << std::endl; // Add this line
        m_selectedObject->setColor(m_selectedObject->getOriginalColor());
        m_selectedObject = nullptr;
    }

    return clickedObject;
}

void Scene::printMatrix(const glm::mat4& matrix) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << matrix[i][j] << "\t";
        }
        std::cout << std::endl;
    }
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


glm::vec2 Scene::calculateNormalizedPosition(const ImVec2& mousePos, int display_w, int display_h) {
    // 1. Convert mouse coordinates to range [0, display_w] and [0, display_h]
    float x = mousePos.x;
    float y = display_h - mousePos.y; // Invert Y (assuming the origin is at the top-left)

    // 2. Normalize to range [0, 1]
    float normalizedX = x / display_w;
    float normalizedY = y / display_h;

    // 3. Remap to NDC range [-1, 1]
    float ndcX = (normalizedX * 2.0f) - 1.0f;
    float ndcY = (normalizedY * 2.0f) - 1.0f;

    return glm::vec2(ndcX, ndcY);
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
