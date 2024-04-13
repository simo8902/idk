//
// Created by Simeon on 4/7/2024.
//

#include "Renderer.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "gtx/string_cast.hpp"

Renderer::Renderer(int width, int height, const char *title, float fov, float aspectRatio, float nearPlane, float farPlane) :
        m_WindowWidth(width), m_WindowHeight(height),
        m_camera(glm::vec3(0.0f, 1.0f, -10.0f),
                 glm::vec3(0.0f, 0.0f, -1.0f),
                 glm::vec3(0.0f, 1.0f, 0.0f), fov, aspectRatio, nearPlane, farPlane)
{
    m_camera.printCameraParams();

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

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
        void* ptr = glfwGetWindowUserPointer(window);
        if (ptr) {
            static_cast<Renderer*>(ptr)->mouseButtonCallback(window, button, action, mods);
        }
    });

    glfwSetWindowUserPointer(m_Window, this); // Do this in your Renderer's constructor or initialization method.
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    initializeImGui(m_Window);


    shaderProgram = Shader::createShaderProgram();
    if (!shaderProgram) {
        std::cerr << "Failed to create shader program." << std::endl;
        return;
    }

    try {
        globalScene = new Scene();
        globalScene->setShader(*shaderProgram);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Failed to allocate memory for scene: " << e.what() << '\n';
        return;
    }

    globalScene->setCamera(m_camera);

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

Ray Renderer::generateRayFromMouse(const glm::vec2& ndc,int display_w,int display_h) {
    glm::vec4 rayStart_NDC(ndc.x, ndc.y, -1.0f, 1.0f);
    glm::vec4 rayEnd_NDC(ndc.x, ndc.y, 0.0f, 1.0f);

    glm::mat4 invProjMatrix = glm::inverse(m_camera.getProjectionMatrix(display_w, display_h));
    glm::mat4 invViewMatrix = glm::inverse(m_camera.getViewMatrix());

    glm::vec4 rayStart_world = invViewMatrix * invProjMatrix * rayStart_NDC;
    rayStart_world /= rayStart_world.w;
    glm::vec4 rayEnd_world = invViewMatrix * invProjMatrix * rayEnd_NDC;
    rayEnd_world /= rayEnd_world.w;

    glm::vec3 rayDir_world(glm::normalize(rayEnd_world - rayStart_world));


    return Ray(glm::vec3(rayStart_world), rayDir_world);
}


void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        glm::vec2 ndc = {
                (2.0f * mouseX) / windowWidth - 1.0f,
                1.0f - (2.0f * mouseY) / windowHeight
        };

        std::cerr << "NDC Coords: "<< glm::to_string(ndc) << std::endl;

        Ray ray = generateRayFromMouse(ndc, windowWidth, windowHeight);
        bool objectSelected = false;
        for (const auto& object : globalScene->getGameObjects()) {
            Transform* transform = object->getComponent<Transform>();
            BoxCollider* collider = object->getComponent<BoxCollider>();

            if (collider && transform) {
                glm::mat4 transformMatrix = transform->getModelMatrix();
                if (collider->intersectsRay(ray, transformMatrix)) {
                    std::cout << "Object selected: " << object->getName() << std::endl;
                    globalScene->selectObject(object);
                    objectSelected = true;
                    break;
                }
            }
        }
        if (!objectSelected) {
            std::cout << "No object selected." << std::endl;
            globalScene->deselectCurrentObject();
        }
    }
}

bool Renderer::ShouldClose() {
    return glfwWindowShouldClose(m_Window);
}

//Main Loop
void Renderer::render(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glfwGetFramebufferSize(m_Window, &m_WindowWidth, &m_WindowHeight);

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

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    globalScene->renderSceneView(m_WindowWidth, m_WindowHeight);

    std::shared_ptr<GameObject> selectedObjectSharedPtr = HierarchyManager::selectedObject ? std::shared_ptr<GameObject>(HierarchyManager::selectedObject) : nullptr;

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    inspectorManager.renderInspector(selectedObjectSharedPtr);
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