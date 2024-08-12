//
// Created by Simeon on 4/7/2024.
//

#include "Renderer.h"

Renderer::Renderer(Shader* shaderProgram, Shader* wireframe, std::shared_ptr<Camera> camera, GLFWwindow* m_Window)
    : shaderProgram(shaderProgram),
        m_Window(m_Window),
        wireframe(wireframe), m_Camera(camera){
    objects = std::make_shared<SubGameObject>("test");


    myRenderer = this;
    createObjects();
}

Renderer::~Renderer() {}

void Renderer::DrawGrid(float gridSize, float gridStep) {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glDrawArrays(GL_LINES, 0, gridVertices.size());

    glDisableVertexAttribArray(0);

    glDeleteBuffers(1, &gridVBO);
}


void Renderer::createSceneFramebuffer(int sceneWidth, int sceneHeight) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sceneWidth, sceneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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

void Renderer::renderSceneView() {
    ImGui::Begin("Scene Viewport", nullptr,
                 ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

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

        createSceneFramebuffer(FBO_width, FBO_height); // Create the framebuffer with the size of the current window
    }

    renderSceneViewport(FBO_width, FBO_height, FBO);

    ImVec2 uv0(0, 1); // Top-left
    ImVec2 uv1(1, 0); // Bottom-right

    ImGui::Image((void *) (intptr_t) texture_id, viewportSize, uv0, uv1);

    ImGui::End();
}

//Main Loop
void Renderer::render() {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderImGuiLayout();
    renderSceneView();
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
    hierarchyManager.renderHierarchy(myRenderer);

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    inspectorManager.renderInspector(HierarchyManager::selectedCamera);

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    projectExplorer.renderProjectExplorer();

    renderToolbar();
}

void Renderer::renderToolbar() {
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoScrollbar |ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SetCursorPosY( 5);
    
    if (ImGui::Button("A")) {
        currentGizmoMode = ImGuizmo::TRANSLATE;
    }
    ImGui::SameLine();

    if (ImGui::Button("B", ImVec2(20, 20))) {
        currentGizmoMode = ImGuizmo::ROTATE;
    }

    ImGui::SameLine();

    if (ImGui::Button("C", ImVec2(20, 20))) {
        currentGizmoMode = ImGuizmo::SCALE;
    }

    ImGui::End();
}

void Renderer::Render3DScene() {
    glm::mat4 view = m_Camera->getViewMatrix();
    glm::mat4 projection = m_Camera->getProjectionMatrix();

    shaderProgram->Use();
    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);

    glm::vec3 objectColor = glm::vec3(0.2f, 0.2f, 0.2f);
    shaderProgram->setVec3("objectColorUniform", objectColor);

    for (const auto &obj: objects->getGameObjects()) {

        auto *transformComponent = obj->getComponent<Transform>();
        auto *boxCollider = obj->getComponent<BoxCollider>();

        if (transformComponent == nullptr) {
            std::cerr << "transform is null\n";
        }
        if (boxCollider == nullptr) {
            std::cerr << "boxCollider is null\n";
        }

        if (transformComponent) {
            glm::mat4 model = transformComponent->getModelMatrix();
            shaderProgram->setMat4("model", model);
        }
        if (obj == selectedObjects && boxCollider && transformComponent) {
            wireframe->Use();
            wireframe->setMat4("m_View", view);
            wireframe->setMat4("m_Projection", projection);

            glm::mat4 wireModel = transformComponent->getModelMatrix();
            wireframe->setMat4("m_Model", wireModel);

            obj->DebugDraw(*wireframe);
        }

        obj->Draw(*shaderProgram);
    }
}



void Renderer::createObjects() {
    //Cube1
    std::shared_ptr<Cube> cube1 = std::make_shared<Cube>("Cube1");
    cube1->addComponent<Transform>();

    Transform *cube1Transform = cube1->getComponent<Transform>();
    cube1Transform->setPosition(glm::vec3(-2.0f, 1.5f, -2.5f));
    cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));

    //Cube2
    std::shared_ptr<Cube> cube2 = std::make_shared<Cube>("Cube2");
    cube2->addComponent<Transform>();

    Transform *cube2Transform = cube2->getComponent<Transform>();
    cube2Transform->setPosition(glm::vec3(2.0f, 1.5f, -2.5f));
    cube2->addComponent<BoxCollider>(cube2Transform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));

    objects->addObject(cube1);
    objects->addObject(cube2);
}

void Renderer::renderSceneViewport(int viewportWidth, int viewportHeight, GLuint framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawGrid(10.0f, 1.0f);
    Render3DScene();

    bool objectSelected = false;
    static bool wasPressed = false;


    bool isPressed = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (isPressed && !wasPressed) {
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        double mouseX, mouseY;
        glfwGetCursorPos(m_Window, &mouseX, &mouseY);

        mouseX -= windowPos.x;
        mouseY -= windowPos.y;

        glm::vec2 ndc = {
                (2.0f * mouseX) / windowSize.x - 1.0f,
                1.0f - (2.0f * mouseY) / windowSize.y
        };

        Ray ray = ray.getRayFromScreenPoint(ndc, m_Camera);

        for (const auto &object: objects->getGameObjects()) {
            Transform *transform = object->getComponent<Transform>();
            BoxCollider *collider = object->getComponent<BoxCollider>();

            if (collider && transform) {
                glm::mat4 transformMatrix = transform->getModelMatrix();
                if (collider->intersectsRay(ray, transformMatrix)) {
                    selectedObjects = object;

                    std::cout << object->getName() << " was selected" << std::endl;
                    objectSelected = true;
                    break;
                }
            }
        }

        if (!objectSelected && mouseX >= 0 && mouseX <= windowSize.x && mouseY >= 0 && mouseY <= windowSize.y) {
            std::cerr << "obj not selected!\n";
            selectedObjects = nullptr;
            objectSelected = false;

        }
    }

    if (glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(m_Window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        if (glfwGetKey(m_Window, GLFW_KEY_C) == GLFW_PRESS) {
            if (selectedObjects != nullptr) {
                selectedSubObject = std::static_pointer_cast<SubGameObject>(selectedObjects);
                if (selectedSubObject) {
                    copiedObject = std::static_pointer_cast<SubGameObject>(selectedSubObject->copyObjects(*selectedObjects));
                    std::cout << "cast successfull\n";
                } else {
                    std::cerr << "Failed to cast selectedObjects to SubGameObject\n";
                }
            }
        }
    }

    if (glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(m_Window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        if (glfwGetKey(m_Window, GLFW_KEY_V) == GLFW_PRESS) {
            if (copiedObject) {
                std::shared_ptr<SubGameObject> pastedObject = std::static_pointer_cast<SubGameObject>(copiedObject->clone());

                copiedObject->addObject(pastedObject);
                std::cout << "Pasted object: " << pastedObject->getName() << std::endl;
            } else {
                std::cerr << "No object has been copied yet\n";
            }
        }
    }

    wasPressed = isPressed;

    /*
    if (selectedObjects->getComponents().size() == copiedObject->getComponents().size()) {
        std::cout << "Copy constructor works correctly." << std::endl;
    } else {
        std::cout << "Copy constructor does not work correctly." << std::endl;
    }*/
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Renderer::renderImGuizmo(){
    ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    /*
    if (selectedObjects) {
        glm::mat4 view = m_Camera->getViewMatrix();
        glm::mat4 projection = m_Camera->getProjectionMatrix();

        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(),
                          ImGui::GetWindowHeight());


        glm::mat4 objectMatrix = selectedObjects->getComponent<Transform>()->getModelMatrix();

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), currentGizmoMode, ImGuizmo::LOCAL,
                             glm::value_ptr(objectMatrix));

        selectedObjects->getComponent<Transform>()->setModelMatrix(objectMatrix);

        inspectorManager.renderInspector(selectedObjects);
    }*/
    ImGui::End();
}