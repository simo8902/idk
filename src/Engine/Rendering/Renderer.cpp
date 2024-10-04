//
// Created by Simeon on 4/7/2024.
//

#include "Renderer.h"

#include <LightManager.h>

#include "CameraManager.h"
#include "imgui_internal.h"
#include "Logger.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include ".h/BoxCollider.h"
#include ".h/Capsule.h"
#include ".h/CylinderCollider.h"
#include ".h/SphereCollider.h"
#include ".h/CapsuleCollider.h"
#include ".h/Sphere.h"
#include "Scene.h"
#include "SelectionManager.h"
#include "IconsFontAwesome6Brands.h"

#include "stb_image.h"
#include "imgui_internal.h"

Renderer::Renderer(Scene* scene, const std::shared_ptr<Camera> & camera,const std::shared_ptr<LightManager> & lightManager, GLFWwindow* m_Window)
    : scene(scene), lightManager(lightManager), m_Window(m_Window), m_Camera(camera)
    , FBO_width(), FBO_height(), FBO(), RBO(), texture_id()
{
    myRenderer = this;

    hierarchyManager.setRenderer(this);
    hierarchyManager.setScene(scene);
    hierarchyManager.setLightManager(lightManager);

    glfwSetWindowUserPointer(m_Window, this);
    setMouseButtonCallback();
    setScrollCallback();
    setCursorPosCallback();
    setKeyCallback();
}

    void Renderer::setMouseButtonCallback() {
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window))) {
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
    }

    void Renderer::setScrollCallback() {
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window))) {
                renderer->m_Camera->processScroll(yOffset);
            }
        });
    }

    void Renderer::setCursorPosCallback() {
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
            if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
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
    }

    void Renderer::setKeyCallback() {
        glfwSetKeyCallback(m_Window, key_callback);
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


void Renderer::processInput(GLFWwindow* window) {
    const float & currentFrame = glfwGetTime();
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

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_Camera->processKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_Camera->processKeyboard(CameraMovement::DOWN, deltaTime);
}

Renderer::~Renderer() = default;

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

}

//Main Loop
void Renderer::render() {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    processInput(m_Window);

    renderImGuiLayout();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(m_Window);

    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Window, true);
    }
}

void Renderer::renderImGuiLayout() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {  }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {  }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {  }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {  }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("DockSpace Window", nullptr, window_flags);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoResize);

    SetupDockingLayout(dockspace_id);

    renderToolbar();
    renderSceneView();
    renderHierarchy();
    renderInspector();
    renderProjectExplorer();
    logger.Display();

    ImGui::End();
}
void Renderer::SetupDockingLayout(ImGuiID dockspace_id)
{
    static bool dock_builder_initialized = false;
    if (!dock_builder_initialized) {
        dock_builder_initialized = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);

        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->WorkSize);

        ImGuiID dock_main_id = dockspace_id;

        ImGuiID dock_toolbar_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.037f, nullptr, &dock_main_id);
        ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

        ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
        ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
        ImGuiID dock_scene_id = dock_main_id;

        ImGui::DockBuilderDockWindow("Toolbar", dock_toolbar_id);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
        ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
        ImGui::DockBuilderDockWindow("Scene Viewport", dock_scene_id);
        ImGui::DockBuilderDockWindow("Console", dock_bottom_id);
        ImGui::DockBuilderDockWindow("Project Explorer", dock_bottom_id);

        ImGui::DockBuilderFinish(dock_main_id);
    }
}


void Renderer::renderSceneView() {
    ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

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

    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture_id)), viewportSize, uv0, uv1);

    renderImGuizmo();

    ImGui::End();
}

void Renderer::renderSceneViewport(int viewportWidth, int viewportHeight, GLuint framebuffer) {
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene->Render3DScene();
    scene->DrawGrid(10.0f, 1.0f);

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseDown(0)) {
        ImVec2 windowPos = ImGui::GetWindowPos();       // Get window position (top-left)
        ImVec2 cursorPos = ImGui::GetMousePos();        // Get the current mouse position
        ImVec2 contentMin = ImGui::GetWindowContentRegionMin();  // Content start (relative to window)
        ImVec2 contentMax = ImGui::GetWindowContentRegionMax();  // Content end (relative to window)

        float mouseX = cursorPos.x - (windowPos.x + contentMin.x);
        float mouseY = cursorPos.y - (windowPos.y + contentMin.y);

        bool mouseInViewport = (mouseX >= 0 && mouseX <= (contentMax.x - contentMin.x) &&
                                mouseY >= 0 && mouseY <= (contentMax.y - contentMin.y));

        if (mouseInViewport) {
            glm::vec2 ndc = {
                (2.0f * mouseX) / (contentMax.x - contentMin.x) - 1.0f,
                1.0f - (2.0f * mouseY) / (contentMax.y - contentMin.y)
            };

            Ray ray = Ray::getRayFromScreenPoint(ndc, m_Camera);
            float closestDistance = std::numeric_limits<float>::max();
            std::shared_ptr<GameObject> closestObject = nullptr;

            for (const auto& object : Scene::objects) {
                auto transform = object->getComponent<Transform>().get();
                if (!transform) {
                    std::cout << "Object has no transform, skipping.\n";
                    continue;
                }

                const glm::mat4& transformMatrix = transform->getModelMatrix();
                float intersectionDistance = 0.0f;
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

            constexpr float lightClickRadius = 0.5f;
            const auto& lights = lightManager->getDirectionalLights();
            for (const auto& light : lights) {
                if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(light)) {
                    glm::vec3 lightPosition = directionalLight->getPosition();

                    float distance = glm::length(lightPosition - ray.m_origin);

                    if (distance <= lightClickRadius) {
                        SelectionManager::getInstance().selectLight(light);
                        return;
                    }
                }
            }

            if (closestObject) {
                std::cout << "Selected Object: " << closestObject->getName() << std::endl;
                SelectionManager::getInstance().selectGameObject(closestObject);
            } else {
                SelectionManager::getInstance().clearSelection();
            }
        } else {
            std::cout << "Mouse input blocked by ImGui or outside viewport." << std::endl;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Renderer::renderImGuizmo() const {
    auto selectedObject = SelectionManager::getInstance().selectedGameObject;
    auto selectedLight = SelectionManager::getInstance().selectedLight;

    glm::mat4 view = m_Camera->getViewMatrix();
    glm::mat4 projection = m_Camera->getProjectionMatrix();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

    if (selectedObject) {
        auto transform = selectedObject->getComponent<Transform>();
        if (transform) {
            glm::mat4 objectMatrix = transform->getModelMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                 currentGizmoOperation, currentGizmoMode, glm::value_ptr(objectMatrix));

            if (ImGuizmo::IsUsing()) {
                transform->setModelMatrix(objectMatrix);
            }
        }
    } else if (selectedLight) {
        auto transform = selectedLight->getComponent<Transform>();
        if (transform) {
            glm::mat4 objectMatrix = transform->getModelMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                 currentGizmoOperation, currentGizmoMode, glm::value_ptr(objectMatrix));

            if (ImGuizmo::IsUsing()) {
                transform->setModelMatrix(objectMatrix);
                selectedLight->updateDirectionFromRotation();
            }
        }
    } else {

        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

        static bool noObjectSelectedLogged = false;

        if (selectedObject) {
            noObjectSelectedLogged = false;

            auto transform = selectedObject->getComponent<Transform>();
            if (transform) {
                glm::mat4 objectMatrix = transform->getModelMatrix();

                ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                     currentGizmoOperation, currentGizmoMode, glm::value_ptr(objectMatrix));

                if (ImGuizmo::IsUsing()) {
                    transform->setModelMatrix(objectMatrix);
                }
            }
        } else if (selectedLight) {
            noObjectSelectedLogged = false;

            auto transform = selectedLight->getComponent<Transform>();
            if (transform) {
                glm::mat4 objectMatrix = transform->getModelMatrix();

                ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                     currentGizmoOperation, currentGizmoMode, glm::value_ptr(objectMatrix));

                if (ImGuizmo::IsUsing()) {
                    transform->setModelMatrix(objectMatrix);
                    selectedLight->updateDirectionFromRotation();
                }
            }
        } else {
            if (!noObjectSelectedLogged) {
                std::cout << "No object or light selected!" << std::endl;
                noObjectSelectedLogged = true;
            }
        }
    }
}

void Renderer::renderToolbar() {
    ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoScrollbar |
                                 ImGuiWindowFlags_NoScrollWithMouse | ImGuiDockNodeFlags_NoTabBar | ImGuiWindowFlags_NoResize;

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);


    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

    if (ImGui::Begin("Toolbar", nullptr, toolbar_flags)) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

        ImGui::PushID("TranslateButton");
        if (ImGui::Button(ICON_FA_ARROWS_ALT)) {
            currentGizmoOperation = ImGuizmo::TRANSLATE;

        }
        ImGui::PopID();
        ImGui::SameLine();

        ImGui::PushID("RotateButton");
        if (ImGui::Button(ICON_FA_SYNC_ALT)) {
            currentGizmoOperation = ImGuizmo::ROTATE;
        }
        ImGui::PopID();
        ImGui::SameLine();

        ImGui::PushID("ScaleButton");
        if (ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT)) {
            currentGizmoOperation = ImGuizmo::SCALE;

        }
        ImGui::PopID();
        ImGui::SameLine();

        ImGui::PushID("LocalTransformButton");
        if (ImGui::Button(ICON_FA_LOCATION_ARROW)) {
            currentGizmoMode = ImGuizmo::LOCAL;
        }
        ImGui::PopID();
        ImGui::SameLine();

        ImGui::PushID("WorldTransformButton");
        if (ImGui::Button(ICON_FA_GLOBE)) {
            currentGizmoMode = ImGuizmo::WORLD;
        }
        ImGui::PopID();
        ImGui::SameLine();

        ImGui::PopStyleVar();
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
}

void Renderer::renderHierarchy() {
    if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::GetStyle().WindowPadding = ImVec2(5, 5);
        ImGui::GetStyle().WindowBorderSize = 1.0f;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));

        hierarchyManager.renderHierarchy();

        ImGui::PopStyleVar(1);
    }
    ImGui::End();
}

void Renderer::renderInspector() {
    if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize))
    {
        inspectorManager.renderInspector();
    }
    ImGui::End();
}

void Renderer::renderProjectExplorer() {
    if (ImGui::Begin("Project Explorer", nullptr, ImGuiWindowFlags_NoResize))
    {
        projectExplorer.renderProjectExplorer();
    }
    ImGui::End();
}


void Renderer::ShowMemoryUsageWindow() {
    ImGui::Begin("Memory Usage", nullptr, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize);

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
GLuint Renderer::loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    std::cout << "Loaded texture: " << path << " | Width: " << width << " | Height: " << height << " | Channels: " << nrChannels << std::endl;

    GLenum format;
    if (nrChannels == 1) {
        format = GL_RED;
    } else if (nrChannels == 3) {
        format = GL_RGB;
    } else if (nrChannels == 4) {
        format = GL_RGBA;
    } else {
        std::cerr << "Unexpected number of channels: " << nrChannels << std::endl;
        stbi_image_free(data);
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

     glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}


void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /*
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
    }*/
}

void Renderer::RenderContextMenu() const {
    if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
        ImGui::OpenPopup("Context Menu");
    }

    if (ImGui::BeginPopup("Context Menu")) {
        if (ImGui::MenuItem("Add Temportal Test Object"))
        {
            Scene::createTemporalObject();
        }

        ImGui::EndPopup();
    }

}

