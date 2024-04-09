//
// Created by Simeon on 4/7/2024.
//

#include "Renderer.h"
#include "../../components/Component.h"
#include "../../components/Transform.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


Renderer::Renderer(int width, int height, const char *title) : m_WindowWidth(width), m_WindowHeight(height)
{
    if (!initializeGLFW()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    m_Window = createGLFWWindow(width, height, title);
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

    initializeImGui(m_Window);

    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    shaderProgram = shaderProgram->createShaderProgram(shaderProgram);
    if (shaderProgram == nullptr) {
        std::cerr << "Shader is null\n";
        return;
    }

    globalScene = new Scene(shaderProgram);

    loader = new SceneLoader(globalScene);
    loader->initialize();
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


// Inside Loop
void Renderer::render() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    glfwGetFramebufferSize(m_Window, &m_WindowWidth, &m_WindowHeight);
    globalScene->calculateProjectionMatrix(m_WindowWidth, m_WindowHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    hierarchyManager.renderHierarchy(globalScene);

    globalScene->update();
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    globalScene->renderSceneView(m_WindowWidth, m_WindowHeight);


    std::shared_ptr<GameObject> selectedObjectSharedPtr = HierarchyManager::selectedObject ? std::shared_ptr<GameObject>(HierarchyManager::selectedObject) : nullptr;

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    inspectorManager.renderInspector(selectedObjectSharedPtr);
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    globalScene->renderGameView();
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    projectExplorer.renderProjectExplorer();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_Window);
    glfwPollEvents();

    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Window, true);
    }

}


void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLFWwindow* Renderer::createGLFWWindow(int width, int height, const char* title) {
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
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