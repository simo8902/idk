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
#include "ShaderManager.h"

Renderer::Renderer(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Camera>
    & camera, GLFWwindow* window)
    :  scene(scene),
    m_Camera(camera),
    m_Window(window)
{
    myRenderer = this;

    hierarchyManager.setRenderer(this);
    hierarchyManager.setScene(scene);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_static);

    setMouseButtonCallback();
    setScrollCallback();
    setCursorPosCallback();
    setKeyCallback();
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



void Renderer::render() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    processInput(m_Window);

    fpsCounter.update();

   // ImGui::ShowDemoWindow();
    renderImGuiLayout();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_context);
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

    /*
    if (m_FirstFrame) {
        if (const ImGuiID dockspace_id = ImGui::GetID("Docky"); ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
            SetupDockingLayout(dockspace_id);
            m_HasLoadedLayout = false;
        } else {
            m_HasLoadedLayout = true;
        }
        m_FirstFrame = false;
    }*/
    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(
           ImGui::GetID("MyPersistentDockSpace"),
           ImGui::GetMainViewport(),
           ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton
       );

    if (m_FirstFrame && ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
        SetupDockingLayout(dockspace_id);
    }
    m_FirstFrame = false;


    renderToolbar();
    renderSceneView();
    renderHierarchy();
    renderInspector();
    renderProjectExplorer(); //TODO: OPTIMIZATIONS NEEDED CPU
}
void Renderer::SetupDockingLayout(const ImGuiID& dockspace_id) {
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGuiID dock_main_id = dockspace_id;
   // ImGui::DockBuilderSetNodeSize(dock_main_id, ImGui::GetMainViewport()->WorkSize);

    const float toolbar_ratio = m_ToolbarHeight / ImGui::GetMainViewport()->WorkSize.y;

    ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, toolbar_ratio, nullptr, &dock_main_id);
    m_TopDockID = dock_top_id;

    ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);

    ImGui::DockBuilderDockWindow("Toolbar", dock_top_id);
    ImGui::DockBuilderDockWindow("Scene Viewport", dock_main_id);
    ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
    ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
    ImGui::DockBuilderDockWindow("Project Explorer", dock_bottom_id);

    ImGui::DockBuilderFinish(dockspace_id);
}

void Renderer::renderToolbar() {
    const ImGuiWindowFlags & toolbar_flags = ImGuiWindowFlags_NoTitleBar |
                                ImGuiWindowFlags_NoCollapse |
                                ImGuiWindowFlags_NoScrollbar |
                                ImGuiWindowFlags_NoScrollWithMouse | ImGuiDockNodeFlags_NoTabBar | ImGuiWindowFlags_NoResize;

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);


    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7, 7));

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

        float windowWidth = ImGui::GetWindowWidth();
        float buttonWidth = 100.0f;
        float centerPosX = (windowWidth - buttonWidth * 2) / 2.0f;

        ImGui::SetCursorPosX(centerPosX);

        ImGui::PushID("PlayButton");
        if (ImGui::Button("Play", ImVec2(buttonWidth, 0))) {
        }
        ImGui::PopID();
        ImGui::SameLine();

        ImGui::PushID("PauseButton");
        if (ImGui::Button("Pause", ImVec2(buttonWidth, 0))) {
        }
        ImGui::PopID();

        ImGui::PopStyleVar();
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, const int &width, const int &height)
{
    framebufferResized = true;
    newWidth = width;
    newHeight = height;
}

void Renderer::renderSceneView() {
    ImGui::Begin("Scene");
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


void Renderer::renderHierarchy() const {
    if (m_HierarchyDockID != 0) {
        ImGui::SetNextWindowDockID(m_HierarchyDockID, ImGuiCond_Always);
    }


    if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::GetStyle().WindowPadding = ImVec2(2, 0);
        ImGui::GetStyle().WindowBorderSize = 1.0f;

       // static char searchBuffer[128] = "";
      //  ImGui::InputTextWithHint("##search", "Търсене...", searchBuffer, IM_ARRAYSIZE(searchBuffer));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 0));

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
            projectExplorer.renderProjectExplorer();
    }
    ImGui::End();
}
void Renderer::ShaderDebugUI() {
    ImGui::Begin("Shader Debug UI", nullptr, ImGuiWindowFlags_NoResize);

    if(ImGui::Button("Reload All")) {
        ShaderManager::Instance().ReloadAll();
    }

    if(ImGui::BeginDragDropTarget()) {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILES")) {
            auto paths = static_cast<const std::vector<std::string>*>(payload->Data);
            ShaderManager::Instance().HandleFileDrop(*paths);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Columns(3, "##shader_columns");
    ImGui::Text("Shader Name"); ImGui::NextColumn();
    ImGui::Text("Status"); ImGui::NextColumn();
    ImGui::Text("Last Modified"); ImGui::NextColumn();
    ImGui::Separator();

    for(const auto& [name, shader] : ShaderManager::Instance().getShaders()) {
        ImGui::Text("%s", name.c_str()); ImGui::NextColumn();
        ImGui::TextColored(shader->isValid() ? ImVec4(0,1,0,1) : ImVec4(1,0,0,1),
                          shader->isValid() ? "Valid" : "Error");
        ImGui::NextColumn();
        ImGui::Text("%s", shader->getLastModified().c_str());
        ImGui::NextColumn();
    }
    ImGui::End();
}


void Renderer::setMouseButtonCallback() const {
    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
         ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

         if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window))) {
             if (!ImGui::GetIO().WantCaptureMouse) {
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
         }
     });
}

void Renderer::setScrollCallback() const {
    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);

            if (!ImGui::GetIO().WantCaptureMouse) {
                if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window))) {
                    renderer->m_Camera->processScroll(yOffset);
                }
            }
        });
}

void Renderer::setCursorPosCallback() const {
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
            ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

            if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window))) {
                if (renderer->m_rightMouseButtonPressed && !ImGui::GetIO().WantCaptureMouse) {
                    if (renderer->m_lastX == 0.0 && renderer->m_lastY == 0.0) {
                        renderer->m_lastX = xpos;
                        renderer->m_lastY = ypos;
                    } else {
                        double xoffset = xpos - renderer->m_lastX;
                        double yoffset = renderer->m_lastY - ypos;
                        renderer->m_lastX = xpos;
                        renderer->m_lastY = ypos;
                        renderer->m_Camera->processMouseMovement(
                            static_cast<float>(xoffset),
                            static_cast<float>(yoffset)
                        );
                    }
                }
            }
        });
}

void Renderer::setKeyCallback() const {
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
          ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

          if (!ImGui::GetIO().WantCaptureKeyboard) {
              if (auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window))) {

              }
          }
      });
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

void Renderer::framebuffer_size_callback_static(GLFWwindow* window, const int width, const int height) {
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        renderer->framebuffer_size_callback(window, width, height);
    }
}
