//
// Created by Simeon on 4/7/2024.
//

#include <algorithm>
#include "Renderer.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include ".h/BoxCollider.h"
#include ".h/Capsule.h"
#include ".h/CylinderCollider.h"
#include ".h/SphereCollider.h"
#include ".h/CapsuleCollider.h"
#include ".h/Sphere.h"
#include "Scene.h"
Renderer::Renderer(Scene* scene, const std::shared_ptr<Camera> & camera, GLFWwindow* m_Window)
    : scene(scene), m_Window(m_Window), m_Camera(camera)
    , FBO(), FBO_height(), FBO_width(), texture_id(), RBO()
{
    myRenderer = this;
    glfwSetWindowUserPointer(m_Window, this);

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods) {
        if (auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window))) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                if (action == GLFW_PRESS && !renderer->m_rightMouseButtonPressed) {
                    renderer->m_rightMouseButtonPressed = true;
                    renderer->m_lastX = 0.0;
                    renderer->m_lastY = 0.0;

                    renderer->m_initialYaw = renderer->m_Camera->getYaw();
                    renderer->m_initialPitch = renderer->m_Camera->getPitch();
                } else if (action == GLFW_RELEASE && renderer->m_rightMouseButtonPressed) {
                    renderer->m_rightMouseButtonPressed = false;
                }
            }
        }
    });


    glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset) {
        if (auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window))) {
            renderer->m_Camera->processScroll(yOffset);
        }
    });


    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos) {
        if (auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
        renderer && renderer->m_rightMouseButtonPressed) {
            if (renderer->m_lastX == 0.0 && renderer->m_lastY == 0.0) {
                renderer->m_lastX = xpos;
                renderer->m_lastY = ypos;
            } else {
                double xoffset = xpos - renderer->m_lastX;
                double yoffset = renderer->m_lastY - ypos;

                renderer->m_lastX = xpos;
                renderer->m_lastY = ypos;

                renderer->m_Camera->processMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
            }
        }
    });

    glfwSetKeyCallback(m_Window, key_callback);

    Scene::createObjects();
}

void Renderer::createSceneFramebuffer(int sceneWidth, int sceneHeight) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWidth, sceneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sceneWidth, sceneHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    std::cerr << "init success\n";
}
void Renderer::setMainCamera(const std::shared_ptr<Camera> & camera) {
    m_Camera = camera;
}

void Renderer::scrollCallback(float yoffset) const {
    if (m_Camera) {
        m_Camera->processMouseScroll(yoffset);
    }else {
        std::cerr << "scrollCallback: m_MainCamera is null!\n";
    }
}

void Renderer::mouseMovementCallback(float xOffset, float yOffset, bool constrainPitch) const {
    if (m_Camera) {
        m_Camera->processMouseMovement(xOffset, yOffset, constrainPitch);
    }
    else {
        std::cerr << "mouseMovementCallback: m_MainCamera is null!\n";
    }
}

Renderer::~Renderer() = default;

void Renderer::renderSceneView() {
    ImGui::Begin("Scene Viewport", nullptr,
                 ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const ImVec2 & viewportSize = ImGui::GetContentRegionAvail();

    if (FBO_width != viewportSize.x || FBO_height != viewportSize.y || FBO == 0 || texture_id == 0) {
        FBO_width = viewportSize.x;
        FBO_height = viewportSize.y;

        if (FBO != 0) {
            glDeleteFramebuffers(1, &FBO);
            FBO = 0;
        }

        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
            texture_id = 0;
        }

        createSceneFramebuffer(FBO_width, FBO_height);
    }

    renderSceneViewport(FBO_width, FBO_height, FBO);

    constexpr ImVec2 uv0(0, 1); // Top-left
    constexpr ImVec2 uv1(1, 0); // Bottom-right

    //ImGui::Image((void *) (intptr_t) texture_id, viewportSize, uv0, uv1);
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture_id)), viewportSize, uv0, uv1);

    ImGui::End();
}

void Renderer::initImGuiStyle() const{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];

    if (isPosButtonActive) {
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color
    } else {
        style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_WindowBg];
    }

    if (isPosButtonActive) {
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color
    } else {
        style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_WindowBg];
    }



//  style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_WindowBg];
    //  style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_WindowBg];

    ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 greyColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    ImVec4 grey2Color = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    ImVec4 blackColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    ImVec4 black2Color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    style.Colors[ImGuiCol_Text] = greyColor;
    style.Colors[ImGuiCol_FrameBg] = grey2Color;
    style.Colors[ImGuiCol_HeaderActive] = grey2Color;
    style.Colors[ImGuiCol_HeaderHovered] = black2Color;
    style.Colors[ImGuiCol_TabActive] = grey2Color;
    style.Colors[ImGuiCol_TabUnfocused] = redColor;
    style.Colors[ImGuiCol_TabUnfocusedActive] = grey2Color;
    style.Colors[ImGuiCol_TabHovered] = grey2Color;
    style.Colors[ImGuiCol_Tab] = redColor;
    style.Colors[ImGuiCol_TitleBgActive] = blackColor;
}

void Renderer::processInput(GLFWwindow* window) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera->processKeyboard(CameraMovement::RIGHT, deltaTime);


}
//Main Loop
void Renderer::render() {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    initImGuiStyle();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    processInput(m_Window);

    renderImGuiLayout();
    renderImGuizmo();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_Window);

    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Window, true);
    }
}

void Renderer::renderImGuiLayout() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {  }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {  }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    renderSceneView();

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    hierarchyManager.renderHierarchy(myRenderer, scene);

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    InspectorManager::renderInspector(HierarchyManager::selectedCamera);

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    projectExplorer.renderProjectExplorer();

    ShowMemoryUsageWindow();
    renderToolbar();
}

void Renderer::ShowMemoryUsageWindow() {
    ImGui::Begin("Memory Usage");

    ImGui::Text("Total Game Objects Memory Usage: %zu bytes", GameObject::GetMemoryUsage());
    ImGui::Text("Capsules Memory Usage: %zu bytes", Capsule::GetMemoryUsage());

    size_t memory = Sphere::GetMemoryUsage();
    float spheresMemory = static_cast<float>(memory) / (1024.0f * 1024.0f);
    ImGui::Text("Spheres Memory Usage: %.2f MB", spheresMemory);

    size_t memoryUsageBytes = SphereCollider::GetMemoryUsage();
    float spheresColliderMemory = static_cast<float>(memoryUsageBytes) / (1024.0f * 1024.0f);
    ImGui::Text("Total Sphere Colliders Memory Usage: %.2f MB", spheresColliderMemory);

    /*
    size_t memUsageBytesCapsule = CapsuleCollider::GetMemoryUsage();
    float capsuleColliderMemory = static_cast<float>(memUsageBytesCapsule) / (1024.0f * 1024.0f);
    ImGui::Text("Total Capsule Colliders Memory Usage: %.2f MB", capsuleColliderMemory);*/

    ImGui::End();
}

void Renderer::renderToolbar() {

    ImGui::Begin("Toolbar", nullptr,
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetCursorPosY(5);

    if (ImGui::Button("Pos")) {
        currentGizmoMode = ImGuizmo::TRANSLATE;
        isPosButtonActive = !isPosButtonActive;
    }

    ImGui::SameLine();

    if (ImGui::Button("Rotate", ImVec2(20, 20))) {
        currentGizmoMode = ImGuizmo::ROTATE;
        isPosButtonActive = !isPosButtonActive;
    }

    ImGui::SameLine();

    if (ImGui::Button("Scale", ImVec2(20, 20))) {
        currentGizmoMode = ImGuizmo::SCALE;
        isPosButtonActive = !isPosButtonActive;
    }

    ImGui::End();
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // Renderer *renderer = (Renderer *) glfwGetWindowUserPointer(window);
    auto *renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL) {
        if (action == GLFW_PRESS) {
            if (renderer->selectedObjects != nullptr){
                renderer->clipboardObject = renderer->selectedObjects;
                if (renderer->clipboardObject) {
                    std::cout << "Object stored in clipboard: " << renderer->clipboardObject->getName() << "\n";
                } else {
                    std::cout << "No object stored in clipboard.\n";
                }
            }
        }
    }

    if ((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_V && action == GLFW_PRESS) {
        if (renderer->clipboardObject) {
            std::shared_ptr<GameObject> pastedObject = renderer->clipboardObject->clone();
            std::string newName = pastedObject->getName() + " - Copied";
            pastedObject->setName(newName);

           // renderer->objects.push_back(pastedObject);
            Scene::objects.push_back(pastedObject);

            std::cout << "Pasted object: " << pastedObject->getName() << std::endl;
        } else {
            std::cerr << "No object in clipboard.\n";
        }
    }
}

void Renderer::renderSceneViewport(int viewportWidth, int viewportHeight, GLuint framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene->DrawGrid(10.0f, 1.0f);
    scene->Render3DScene();

    const bool isPressed = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    static bool wasPressed = false;

if (isPressed && !wasPressed) {
    const ImVec2 windowPos = ImGui::GetWindowPos();
    const ImVec2 windowSize = ImGui::GetWindowSize();

    double mouseX, mouseY;
    glfwGetCursorPos(m_Window, &mouseX, &mouseY);

    const bool mouseInWindow = (mouseX >= windowPos.x && mouseX <= windowPos.x + windowSize.x &&
                          mouseY >= windowPos.y && mouseY <= windowPos.y + windowSize.y);

    if (mouseInWindow) {
        mouseX -= windowPos.x;
        mouseY -= windowPos.y;

        const glm::vec2 & ndc = {
            (2.0f * mouseX) / windowSize.x - 1.0f,
            1.0f - (2.0f * mouseY) / windowSize.y
        };

        const Ray & ray = Ray::getRayFromScreenPoint(ndc, m_Camera);

        float closestDistance = std::numeric_limits<float>::max();
        std::shared_ptr<GameObject> closestObject = nullptr;

        for (const auto& object : Scene::objects) {
            const Transform* transform = object->getComponent<Transform>();
            if (!transform) continue;

            glm::mat4 transformMatrix = transform->getModelMatrix();
            float intersectionDistance;
            bool intersects = false;

            if (auto boxCollider = object->getComponent<BoxCollider>()) {
                intersects = boxCollider->intersectsRay(ray, transformMatrix, intersectionDistance);
            } else if (auto capsuleCollider = object->getComponent<CapsuleCollider>()) {
                intersects = capsuleCollider->intersectsRay(ray, transformMatrix, intersectionDistance);
            } else if (auto cylinderCollider = object->getComponent<CylinderCollider>()) {
                intersects = cylinderCollider->intersectsRay(ray, transformMatrix, intersectionDistance);
            } else if (auto sphereCollider = object->getComponent<SphereCollider>()) {
                intersects = sphereCollider->intersectsRay(ray, transformMatrix, intersectionDistance);
            }


            if (intersects && intersectionDistance < closestDistance) {
                closestDistance = intersectionDistance;
                closestObject = object;
            }
        }

        if (closestObject) {
            if (selectedObjects != closestObject) {
                selectObject(closestObject);
                std::cout << closestObject->getName() << " was selected" << std::endl;
            }
        } else {
            // Deselect if no object is hit and there was a previously selected object
            if (selectedObjects) {
                selectObject(nullptr);
                std::cout << "No object selected, deselecting previous object" << std::endl;
            }
        }
    } else {
        // If clicked outside the window, deselect the current object
        if (selectedObjects) {
          //  selectObject(nullptr);
            std::cout << "Clicked outside window, deselecting object" << std::endl;
        }
    }
}

wasPressed = isPressed;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Renderer::renderImGuizmo() const {
    ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    if (selectedObjects) {
        glm::mat4 view = m_Camera->getViewMatrix();
        glm::mat4 projection = m_Camera->getProjectionMatrix();

        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

        glm::mat4 objectMatrix = selectedObjects->getComponent<Transform>()->getModelMatrix();
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), currentGizmoMode, ImGuizmo::WORLD, glm::value_ptr(objectMatrix));

        selectedObjects->getComponent<Transform>()->setModelMatrix(objectMatrix);

        InspectorManager::renderInspector(selectedObjects);
    }
    ImGui::End();
}
