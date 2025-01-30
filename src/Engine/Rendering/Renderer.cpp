//
// Created by Simeon on 4/7/2024.
//

#include "Renderer.h"

#include <SelectionManager.h>

#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "BoxCollider.h"
#include "Scene.h"
#include "IconsFontAwesome6Brands.h"

Renderer::Renderer(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Camera> & camera,const std::shared_ptr<LightManager> & lightManager, GLFWwindow* window)
    :  lightManager(lightManager), scene(scene),
    m_Camera(camera),
    m_Window(window)
{
    myRenderer = this;

    hierarchyManager.setRenderer(this);
    hierarchyManager.setScene(scene);
  //  hierarchyManager.setLightManager(lightManager);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, Renderer::framebuffer_size_callback_static);

    setMouseButtonCallback();
    setScrollCallback();
    setCursorPosCallback();
    setKeyCallback();
}

void Renderer::framebuffer_size_callback_static(GLFWwindow* window, const int width, const int height) {
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        renderer->framebuffer_size_callback(window, width, height);
    }
}

Renderer::~Renderer()
{
    myRenderer = nullptr;
    m_Window = nullptr;

    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
    }

    if (RBO) {
        glDeleteRenderbuffers(1, &RBO);
        RBO = 0;
    }

    if (texture_id) {
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
}

void Renderer::setMouseButtonCallback() const {
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

void Renderer::setScrollCallback() const {
    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
        if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window))) {
            renderer->m_Camera->processScroll(yOffset);
        }
    });
}

void Renderer::setCursorPosCallback() const {
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

void Renderer::setKeyCallback() const {
    glfwSetKeyCallback(m_Window, key_callback);
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{}

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


void Renderer::render() {
   // std::cout << "[Renderer::render] Rendering in thread " << std::this_thread::get_id() << std::endl;
    glfwPollEvents();

    fpsCounter.update();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    processInput(m_Window);

   // ImGui::ShowDemoWindow();
    renderImGuiLayout();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   if (const ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
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
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

    SetupDockingLayout(dockspace_id);

    renderToolbar();
    renderSceneView();
    renderHierarchy();
    renderInspector();
    renderProjectExplorer();

    ImGui::End();
}
void Renderer::SetupDockingLayout(const ImGuiID & dockspace_id)
{
    static bool dock_builder_initialized = false;
    if (!dock_builder_initialized) {
        dock_builder_initialized = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);

        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->WorkSize);

        ImGuiID dock_main_id = dockspace_id;

        const ImGuiID & dock_toolbar_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.037f, nullptr, &dock_main_id);
        const ImGuiID & dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

        const ImGuiID & dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
        const ImGuiID & dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
        const ImGuiID & dock_scene_id = dock_main_id;

        ImGui::DockBuilderDockWindow("Toolbar", dock_toolbar_id);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
        ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
        ImGui::DockBuilderDockWindow("Scene Viewport", dock_scene_id);
        ImGui::DockBuilderDockWindow("Console", dock_bottom_id);
        ImGui::DockBuilderDockWindow("Project Explorer", dock_bottom_id);

        ImGui::DockBuilderFinish(dock_main_id);
    }
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, const int &width, const int &height)
{
    framebufferResized = true;
    newWidth = width;
    newHeight = height;
}

void Renderer::renderSceneView() {
    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
    const ImVec2 & viewportSize = ImGui::GetContentRegionAvail();

    const int & currentWidth = static_cast<int>(viewportSize.x);
    const int & currentHeight = static_cast<int>(viewportSize.y);

    if (framebufferResized || FBO_width != currentWidth || FBO_height != currentHeight || FBO == 0 || texture_id == 0) {
        FBO_width = currentWidth;
        FBO_height = currentHeight;

        if (FBO != 0) {
            glDeleteFramebuffers(1, &FBO);
            FBO = 0;
        }

        scene->updateViewportFramebuffer(FBO_width, FBO_height);

        framebufferResized = false;
    }

    scene->RenderGeometryPass();
    scene->RenderLightingPass();
    scene->RenderFinalPass();

    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(scene->lightingTexture)), viewportSize);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 imagePos = ImGui::GetItemRectMin();
    const auto textPos = ImVec2(imagePos.x + 150, imagePos.y + 25);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), ("FPS: " + std::to_string(fpsCounter.getFPS())).c_str());

    /*
    float smallSize = 450.0f;
    ImVec2 debugSize(smallSize, smallSize);


    ImGui::Text("G-Buffer Textures:");
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(scene->gPosition)), debugSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::SameLine();
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(scene->gNormal)), debugSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::SameLine();
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(scene->gAlbedoSpec)), debugSize, ImVec2(0, 1), ImVec2(1, 0));
*/
    renderImGuizmo();

    ImGui::End();
}

void Renderer::renderImGuizmo() const {
    const auto & selectionManager = SelectionManager::getInstance();
    const auto & selectedComponent = selectionManager.getSelectedComponent();
    const auto & selectedLight = selectionManager.getSelectedLight();

    glm::mat4 view = m_Camera->getViewMatrix();
    glm::mat4 projection = m_Camera->getProjectionMatrix();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    const auto & windowPos = ImGui::GetWindowPos();
    const auto & windowSize = ImGui::GetWindowSize();
    ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

    if (selectedComponent) {
        const auto & transform = selectedComponent->getComponent<Transform>();
        if (transform) {
            glm::mat4 objectMatrix = transform->getModelMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                 currentGizmoOperation, currentGizmoMode, glm::value_ptr(objectMatrix));

            if (ImGuizmo::IsUsing()) {
                transform->setModelMatrix(objectMatrix);
            }
        }
    } else if (selectedLight) {
        const auto & transform = selectedLight->getComponent<Transform>();
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

        if (selectedComponent) {
            noObjectSelectedLogged = false;

            const auto & transform = selectedComponent->getComponent<Transform>();
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

            const auto & transform = selectedLight->getComponent<Transform>();
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
              //  std::cout << "No object or light selected!" << std::endl;
                noObjectSelectedLogged = true;
            }
        }
    }
}

void Renderer::renderToolbar() {
    const ImGuiWindowFlags & toolbar_flags = ImGuiWindowFlags_NoTitleBar |
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

void Renderer::renderHierarchy() const {
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
    if (ImGui::Begin("Inspector"))
    {
        inspectorManager.renderInspector();
    }
    ImGui::End();
}

void Renderer::renderProjectExplorer() {
    if (ImGui::Begin("Project Explorer", nullptr, ImGuiWindowFlags_NoResize))
    {
       // projectExplorer.renderProjectExplorer();
    }
    ImGui::End();
}
