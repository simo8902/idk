//
// Created by Simeon on 4/27/2024.
//

#include "Initialization.h"

#include <IconsFontAwesome6Brands.h>

#include "CameraManager.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "LightManager.h"
#include "Shader.h"
#include "AssetManager.h"
#include "boost/uuid.hpp"

namespace fs = std::filesystem;

Initialization::Initialization(GLFWwindow* window)
    : scene(nullptr), lightManager(nullptr), m_Window(window), m_Renderer(nullptr){
    try {
        std::cout << "[Initialization] Starting Initialization in thread "
                  << std::this_thread::get_id() << std::endl;

        const fs::path engineDir = SOURCE_DIR;
        if (!fs::exists(engineDir))
        {
            std::cerr << "[Initialization] Engine directory does not exist: " << engineDir << std::endl;
            throw std::runtime_error("Engine directory does not exist.");
        }
        current_path(engineDir);

        auto shaderProgram = std::make_shared<Shader>(SOURCE_DIR "/src/shaders/basic.vert", SOURCE_DIR "/src/shaders/basic.frag", "shaderProgram");
        auto lightShader = std::make_shared<Shader>(SOURCE_DIR "/src/shaders/lightShader.vert", SOURCE_DIR "/src/shaders/lightShader.frag", "lightShader");
        auto finalPassShader = std::make_shared<Shader>(SOURCE_DIR "/src/shaders/finalPass.vert", SOURCE_DIR "/src/shaders/finalPass.frag", "finalPassShader");
        assets.push_back(shaderProgram);
        assets.push_back(lightShader);
        assets.push_back(finalPassShader);

        for (const auto& asset : assets) {
            asset->printID();
        }
        cameraInit();
        glfwSetWindowUserPointer(m_Window, this);

        lightManager = std::make_shared<LightManager>();

        glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
        const auto& directionalLight = std::make_shared<DirectionalLight>(
            "Main Light",
            lightDirection,
            glm::vec3(0.9f, 0.9f, 0.9f),  // Ambient
            glm::vec3(1.0f, 1.0f, 1.0f),  // Diffuse
            glm::vec3(1.0f, 1.0f, 1.0f)   // Specular
        );
        directionalLight->updateDirectionFromRotation();

       lightManager->addDirectionalLight(directionalLight);

        std::cout << "[INIT] Light manager: " << (lightManager ? "valid" : "null") << std::endl;
        std::cout << "[INIT] Camera: " << (m_MainCamera ? "valid" : "null") << std::endl;

        scene = std::make_shared<Scene>(shaderProgram, lightShader,finalPassShader,m_MainCamera, lightManager);
        m_Renderer = std::make_shared<Renderer>(scene, m_MainCamera, lightManager, m_Window);

        initializeImGui(m_Window);
    }
    catch (const std::exception& e) {
        std::cerr << "[Initialization] Exception caught in constructor: " << e.what()
                          << " in thread " << std::this_thread::get_id() << std::endl;
        throw;
    }
}

Initialization::~Initialization(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

std::shared_ptr<LightManager> Initialization::getLightManager() const {
    return lightManager;
}

const std::shared_ptr<Scene> & Initialization::getScene() const {
    return scene;
}

GLFWwindow * Initialization::getWindow() const {
    return m_Window;
}

void Initialization::cameraInit() {
    glm::vec3 position = glm::vec3(0.5f, 3.0f, -7.0f);
    glm::vec3 forward = glm::vec3(-0.026f, -0.0471f, 0.99f);
    glm::vec3 up = glm::vec3(-0.0012f, 1.0f, 0.047f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float moveSpeed = 3.0f;
    float mouseSensitivity = 0.3f;
    float fov = 70.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    const std::string name = "Main Camera";
    m_MainCamera = std::make_shared<Camera>(name, position, forward, up, yaw, pitch, moveSpeed, mouseSensitivity, fov, nearPlane, farPlane);
}

void Initialization::initializeImGui(GLFWwindow *window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.IniFilename = SOURCE_DIR "/src/imgui.ini";

    io.Fonts->Clear();

    const auto &fontPath = SOURCE_DIR "/src/data/fonts/CascadiaCode-Bold.ttf";
    const ImFont* primaryFont = io.Fonts->AddFontFromFileTTF(fontPath, 17.0f);
    if (primaryFont) {
       // std::cout << "Successfully loaded primary font: " << fontPath << std::endl;
    } else {
        std::cerr << "Failed to load primary font: " << fontPath << std::endl;
    }

    static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;

    const auto fontAwesomePath = SOURCE_DIR "/src/data/fonts/Font Awesome 6 Free-Solid-900.otf";
    const ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF(fontAwesomePath, 17.0f, &config, icons_ranges);
    if (fontAwesome) {
       // std::cout << "Successfully loaded FontAwesome: " << fontAwesomePath << std::endl;
    } else {
        std::cerr << "Failed to load FontAwesome: " << fontAwesomePath << std::endl;
    }

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

    initImGuiStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

bool Initialization::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Initialization::runMainLoop() const {
    while (!ShouldClose()) {
        if (m_Renderer != nullptr){
            m_Renderer->render();
        }else{
            std::cerr << "m_Renderer is null\n";
        }
    }
}

void Initialization::initImGuiStyle() {
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(0, 0);

    constexpr auto RED = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    constexpr auto BLACK = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    constexpr auto DARK_GREY = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    constexpr auto TEXT = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    constexpr auto FRAME = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
    constexpr auto TOOLBAR_BG = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

    // BG and frame
    style.Colors[ImGuiCol_WindowBg] = DARK_GREY;

    style.Colors[ImGuiCol_FrameBg] = BLACK;
    style.Colors[ImGuiCol_FrameBgHovered] = RED;
    style.Colors[ImGuiCol_FrameBgActive] = RED;

    // Title and border
    style.Colors[ImGuiCol_TitleBg] = BLACK;
    style.Colors[ImGuiCol_TitleBgCollapsed] = BLACK;
    style.Colors[ImGuiCol_TitleBgActive] = BLACK;
    style.Colors[ImGuiCol_Border] = FRAME;

    // Toolbar and menubar
    style.Colors[ImGuiCol_MenuBarBg] = TOOLBAR_BG;
    style.Colors[ImGuiCol_Tab] = BLACK;
    style.Colors[ImGuiCol_TabHovered] = RED;
    style.Colors[ImGuiCol_TabSelected] = BLACK;
    style.Colors[ImGuiCol_TabDimmedSelected] = DARK_GREY;
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = BLACK;
    style.Colors[ImGuiCol_TabSelectedOverline] = BLACK;

    // Buttons and headers
    style.Colors[ImGuiCol_Button] = BLACK;
    style.Colors[ImGuiCol_ButtonHovered] = RED;
    style.Colors[ImGuiCol_ButtonActive] = RED;
    style.Colors[ImGuiCol_Header] = BLACK;
    style.Colors[ImGuiCol_HeaderHovered] = BLACK;
    style.Colors[ImGuiCol_HeaderActive] = BLACK;

    // Resize grips
    style.Colors[ImGuiCol_ResizeGrip] = FRAME;
    style.Colors[ImGuiCol_ResizeGripActive] = FRAME;
    style.Colors[ImGuiCol_ResizeGripHovered] = FRAME;

    // Text and others
    style.Colors[ImGuiCol_Text] = TEXT;
    style.Colors[ImGuiCol_TextDisabled] = RED;
    style.Colors[ImGuiCol_BorderShadow] = BLACK;

    style.WindowMenuButtonPosition = ImGuiDir_None;

    style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];
}