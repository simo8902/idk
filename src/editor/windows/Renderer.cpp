//
// Created by Simeon on 4/7/2024.
//

#include "Renderer.h"
#include "GLFW/glfw3.h"

#include "ImGuizmo.h"
#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "gtx/string_cast.hpp"
#include "../../Cube.h"
#include "../../components/colliders/BoxCollider.h"


Renderer::Renderer(){
    initialization();
    myRenderer = this;

    if (!initializeGLFW()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    m_Window = createGLFWWindow(1280, 720);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    if (!initializeOpenGL()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(m_Window, this);

    shaderProgram = new Shader(SOURCE_DIR "/shaders/basicVertex.glsl", SOURCE_DIR "/shaders/basicFragment.glsl");
    wireframe = new Shader(SOURCE_DIR "/shaders/wireframeVert.glsl", SOURCE_DIR "/shaders/wireframeFrag.glsl");

    initializeImGui(m_Window);

    try {
        globalScene = new Scene();
        globalScene->setScene(*globalScene);
        globalScene->setShader(*shaderProgram);
    } catch (const std::bad_alloc &e) {
        std::cerr << "Failed to allocate memory for scene: " << e.what() << '\n';
        return;
    }

    globalScene->setCamera(*m_Camera);
    initialize();

}

Renderer::~Renderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool Renderer::ShouldClose() {
    return glfwWindowShouldClose(m_Window);
}

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

    // Delete the VBO if you won't need the grid again
    glDeleteBuffers(1, &gridVBO);
}

Ray Renderer::getRayFromScreenPoint(glm::vec2 ndc) {
    glm::vec4 rayStart_NDC(ndc.x, ndc.y, -1.0f, 1.0f);
    glm::vec4 rayEnd_NDC(ndc.x, ndc.y, 0.0f, 1.0f);

    glm::mat4 currentProjection = m_Camera->getProjectionMatrix();
    glm::mat4 currentView = m_Camera->getViewMatrix();

    glm::mat4 invProjMatrix = glm::inverse(currentProjection);
    glm::mat4 invViewMatrix = glm::inverse(currentView);

    glm::vec4 rayStart_world = invViewMatrix * invProjMatrix * rayStart_NDC;
    rayStart_world /= rayStart_world.w;
    glm::vec4 rayEnd_world = invViewMatrix * invProjMatrix * rayEnd_NDC;
    rayEnd_world /= rayEnd_world.w;

    glm::vec3 rayOrigin = m_Camera->getPosition();
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayEnd_world) - glm::vec3(rayStart_world));

    return Ray(rayOrigin, rayDirection);
}

void Renderer::initialize() {
    //Cube1
    std::shared_ptr<Cube> cube1 = objects->addObject<Cube>("Cube1");
    cube1->addComponent<Transform>();

    Transform *cube1Transform = cube1->getComponent<Transform>();
    cube1Transform->setPosition(glm::vec3(-2.0f, 1.5f, -2.5f));
    cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));

    //Cube2
    std::shared_ptr<Cube> cube2 = objects->addObject<Cube>("Cube2");
    cube2->addComponent<Transform>();

    Transform *cube2Transform = cube2->getComponent<Transform>();
    cube2Transform->setPosition(glm::vec3(2.0f, 1.5f, -2.5f));
    cube2->addComponent<BoxCollider>(cube2Transform->getPosition(), glm::vec3(-0.6f), glm::vec3(0.6f));

    m_objects.push_back(cube1);
    m_objects.push_back(cube2);
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

void Renderer::Render3DScene() {
    glm::mat4 view = m_Camera->getViewMatrix();
    glm::mat4 projection = m_Camera->getProjectionMatrix();

    shaderProgram->Use();
    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);

    glm::vec3 objectColor = glm::vec3(0.2f, 0.2f, 0.2f);
    shaderProgram->setVec3("objectColorUniform", objectColor);

    for (const auto &obj: m_objects) {

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

        if (boxCollider && transformComponent) {
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

void Renderer::renderSceneView() {
    ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );

    ImVec2 windowSize2 = ImGui::GetWindowSize();

    if (FBO_width != windowSize2.x || FBO_height != windowSize2.y) {
        FBO_width = windowSize2.x;
        FBO_height = windowSize2.y;

        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture_id);
        glGenTextures(1, &texture_id);
        createSceneFramebuffer(FBO_width, FBO_height);
    }

    renderSceneViewport(FBO_width, FBO_height, FBO);

    glViewport(0, 0, (int)windowSize2.x, (int)windowSize2.y);
    ImGui::Image((void *) (intptr_t) texture_id, windowSize2, ImVec2(0, 1), ImVec2(1, 0));

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

GLFWwindow *Renderer::createGLFWWindow(int width, int height) {
    GLFWwindow *window = glfwCreateWindow(width, height, "LupusFire_core", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

void errorCallback(int error, const char *description) {
    std::cerr << "Error: " << error << " " << description << std::endl;
}

bool Renderer::initializeGLFW() {
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    return true;
}

bool Renderer::initializeOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return false;
    }
    return true;
}

void Renderer::initializeImGui(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.Fonts->Clear();

    const char *fontPath = SOURCE_DIR "/CascadiaCode.ttf";
    if (io.Fonts->AddFontFromFileTTF(fontPath, 17.0f)) {
        io.Fonts->Build();
    } else {
        std::cerr << strerror(errno) << std::endl;
    }
    io.Fonts->Build();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;

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

void Renderer::renderImGuiLayout() {
    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    hierarchyManager.renderHierarchy(myRenderer);
    //TODO: Implement last selected object

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    inspectorManager.renderInspector(HierarchyManager::selectedCamera);

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    projectExplorer.renderProjectExplorer();

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

        Ray ray = getRayFromScreenPoint(ndc);

        for (const auto &object: m_objects) {
            Transform *transform = object->getComponent<Transform>();
            BoxCollider *collider = object->getComponent<BoxCollider>();

            if (collider && transform) {
                glm::mat4 transformMatrix = transform->getModelMatrix();
                if (collider->intersectsRay(ray, transformMatrix)) {
                    selectedObject = object;

                    std::cout << object->getName() << " was selected" << std::endl;
                    objectSelected = true;
                    break;
                }
            }
        }

        if (!objectSelected && mouseX >= 0 && mouseX <= windowSize.x && mouseY >= 0 && mouseY <= windowSize.y) {
            std::cerr << "obj not selected!\n";
        }

    }

    wasPressed = isPressed;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}


void Renderer::renderImGuizmo(){
    ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    if (selectedObject) {
        glm::mat4 view = m_Camera->getViewMatrix();
        glm::mat4 projection = m_Camera->getProjectionMatrix();

        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(),
                          ImGui::GetWindowHeight());


        glm::mat4 objectMatrix = selectedObject->getComponent<Transform>()->getModelMatrix();

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), ImGuizmo::TRANSLATE, ImGuizmo::LOCAL,
                             glm::value_ptr(objectMatrix));

        selectedObject->getComponent<Transform>()->setModelMatrix(objectMatrix);

        inspectorManager.renderInspector(selectedObject);
    }
    ImGui::End();
}