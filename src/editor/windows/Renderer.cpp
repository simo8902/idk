//
// Created by Simeon on 4/7/2024.
//

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "gtx/string_cast.hpp"

Renderer::Renderer(){
    initialization(1280, 720, "LupusFire", 90.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    myRenderer = this;
    gridTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    m_Camera->printCameraParams();

    if (!initializeGLFW()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    m_Window = createGLFWWindow(m_width, m_height);
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
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    initializeImGui(m_Window);


    shaderProgram = Shader::createShaderProgram();
    if (!shaderProgram) {
        std::cerr << "Failed to create shader program." << std::endl;
        return;
    }

    try {
        globalScene = new Scene();
        globalScene->setScene(*globalScene);
        std::cerr << globalScene << std::endl;
        globalScene->setShader(*shaderProgram);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Failed to allocate memory for scene: " << e.what() << '\n';
        return;
    }

    globalScene->setCamera(*m_Camera);
    globalScene->setWindow(m_Window);
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


void Renderer::Render3DScene() {
    glViewport(0, 0, static_cast<int>(display_w), static_cast<int>(display_h));

    if (shaderProgram == nullptr) {
        std::cout << "m_shader is not initialized!" << std::endl;
        return;
    }

    shaderProgram->Use();

    glm::mat4 view = m_Camera->getViewMatrix();
    glm::mat4 projection = m_Camera->getProjectionMatrix(display_w, display_h);

    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);

    for ( auto& obj : m_objects) {

        auto *transformComponent = obj->getComponent<Transform>();
        auto *boxCollider = obj->getComponent<BoxCollider>();

        if (transformComponent == nullptr){
            std::cerr << "transform is null\n";
        }
        if(boxCollider == nullptr){
            std::cerr << "boxCollider is null\n";
        }

        if (transformComponent && boxCollider) {
            //   boxCollider->setPosition(transformComponent->getPosition());

            glm::mat4 model = transformComponent->getModelMatrix();
            shaderProgram->setMat4("model", model);
        }

        if (boxCollider){
            obj->DebugDraw(*shaderProgram);
        }
        obj->Draw(*shaderProgram, transformComponent->getModelMatrix());
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glFlush();
}



void Renderer::initialize(){
    //CUBE1
    std::shared_ptr<Cube> cube1 = object->addObject<Cube>("Cube1");
    cube1->addComponent<Transform>();
     cube1->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

    Transform* cube1Transform = cube1->getComponent<Transform>();

    cube1->addComponent<BoxCollider>(cube1Transform->getPosition(), glm::vec3(-1.0f), glm::vec3(1.0f));
    cube1Transform->setPosition(glm::vec3(1.0f, 1.5f, -2.5f));

    //CUBE2
    std::shared_ptr<Cube> cube2 = object->addObject<Cube>("Cube2");
    cube2->addComponent<Transform>();
    //   cube2->setColor(glm::vec3(1.0f, 1.0f, 1.0f));

    Transform* cube2Transform = cube2->getComponent<Transform>();

    cube2->addComponent<BoxCollider>(cube2Transform->getPosition(), glm::vec3(-1.0f), glm::vec3(1.0f));
    cube2Transform->setPosition(glm::vec3(-1.0f, 1.5f, -2.5f));

    addGameObject(cube1);
    addGameObject(cube2);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glDrawArrays(GL_LINES, 0, gridVertices.size());

    glDisableVertexAttribArray(0);

    // Delete the VBO if you won't need the grid again
    glDeleteBuffers(1, &gridVBO);
}

void Renderer::renderSceneView() {
    glViewport(0, 0, display_w, display_h);
    ImGui::Begin("Scene View");

    // Only resize the fbo if the display size has actually changed
    if (FBO_width != display_w || FBO_height != display_h) {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture_id);
        create_framebuffer();
        FBO_width = display_w;
        FBO_height = display_h;
    }

    if (!depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
        depthTestEnabled = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear( GL_DEPTH_BUFFER_BIT);

    DrawGrid(10.0f, 1.0f);
    Render3DScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, display_w, display_h); // Restore viewport

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::Image((void*)(intptr_t)texture_id, windowSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

void Renderer::create_framebuffer() {
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

Ray Renderer::generateRayFromMouse(const glm::vec2& ndc) {
    glm::vec4 rayStart_NDC(ndc.x, ndc.y, -1.0f, 1.0f);
    glm::vec4 rayEnd_NDC(ndc.x, ndc.y, 1.0f, 1.0f);

    glm::mat4 currentProjection = m_Camera->getProjectionMatrix(display_w, display_h);
    glm::mat4 currentView = m_Camera->getViewMatrix();

    glm::mat4 invProjMatrix = glm::inverse(currentProjection);
    glm::mat4 invViewMatrix = glm::inverse(currentView);

    glm::vec4 rayStart_world = invViewMatrix * invProjMatrix * rayStart_NDC;
    rayStart_world /= rayStart_world.w;
    glm::vec4 rayEnd_world = invViewMatrix * invProjMatrix * rayEnd_NDC;
    rayEnd_world /= rayEnd_world.w;

    glm::vec3 rayDir_world(glm::normalize(rayEnd_world - rayStart_world));

    return Ray(glm::vec3(rayStart_world), rayDir_world);
}

struct Line {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 color;
};
std::vector<Line> lines;


//Main Loop
void Renderer::render(){
    glfwPollEvents();


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glfwGetFramebufferSize(m_Window, &display_w, &display_h);

    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save", "Ctrl+S"))   {}
            if (ImGui::MenuItem("Close", "Ctrl+W"))  {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Render panels
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    hierarchyManager.renderHierarchy(myRenderer);

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    renderSceneView();

    std::shared_ptr<GameObject> selectedObjectSharedPtr = HierarchyManager::selectedObject ? std::shared_ptr<GameObject>(HierarchyManager::selectedObject) : nullptr;

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    inspectorManager.renderInspector(selectedObjectSharedPtr);
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    projectExplorer.renderProjectExplorer();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(m_Window, &mouseX, &mouseY);
        glm::vec2 ndc = {
                (mouseX / display_w) * 2.0f - 1.0f,
                1.0f - (mouseY / display_h) * 2.0f
        };

        Ray ray = generateRayFromMouse(ndc);
        lines.push_back({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 10.0f), glm::vec3(1.0f, 1.0f, 0.0f)});
        //camera.handleMouseInput(mouseX, mouseY);
        bool objectSelected = false;

        for (const auto &object: m_objects) {
            Transform *transform = object->getComponent<Transform>();
            BoxCollider *collider = object->getComponent<BoxCollider>();

            if (collider && transform) {
                glm::mat4 transformMatrix = transform->getModelMatrix();
                if (collider->intersectsRay(ray, transformMatrix)) {
                    objectSelected = true;
                    if (objectSelected == true) {
                        std::cout << "Object selected: " << object->getName() << std::endl;
                    }
                    break;
                }
            }
        }
        if (!objectSelected) {
            std::cerr << "obj not selected!\n";
        }
    }

    glfwSwapBuffers(m_Window);

    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Window, true);
    }

}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLFWwindow* Renderer::createGLFWWindow(int width, int height) {
    GLFWwindow* window = glfwCreateWindow(width, height, m_title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

void errorCallback(int error, const char* description) {
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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return false;
    }
    return true;
}

void Renderer::initializeImGui(GLFWwindow* window) {
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
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;

    ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 greyColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    ImVec4 grey2Color = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    ImVec4 blackColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    ImVec4 black2Color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    ImVec4 redColorHovered = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
    ImVec4 redColorActive = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
    //  style.Colors[ImGuiCol_Border] = redColor;
    style.Colors[ImGuiCol_Text] = greyColor;
    // style.Colors[ImGuiCol_TextDisabled] = greyColor;

    style.Colors[ImGuiCol_FrameBg] = grey2Color;
    //   style.Colors[ImGuiCol_FrameBgHovered] = redColor;
//    style.Colors[ImGuiCol_FrameBgActive] = redColor;
    style.Colors[ImGuiCol_HeaderActive] = grey2Color;
    style.Colors[ImGuiCol_HeaderHovered] = black2Color;
    style.Colors[ImGuiCol_TabActive] = grey2Color;
    style.Colors[ImGuiCol_TabUnfocused] = redColor;

    style.Colors[ImGuiCol_TabUnfocusedActive] = grey2Color;
    style.Colors[ImGuiCol_TabHovered] = grey2Color;
    style.Colors[ImGuiCol_Tab] = redColor;
//  style.Colors[ImGuiCol_TitleBg] = redColor;
    style.Colors[ImGuiCol_TitleBgActive] = blackColor;
//  style.Colors[ImGuiCol_TitleBgCollapsed] = redColorActive;


}
