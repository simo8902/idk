//
// Created by Simeon on 4/27/2024.
//

#include "Initialization.h"

#include <IconsFontAwesome6Brands.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "LightManager.h"
#include "Shader.h"
#include "AssetManager.h"
#include "ShaderManager.h"
#include "boost/uuid.hpp"
namespace fs = std::filesystem;

#define LIBDATA_API
extern "C" LIBDATA_API void imguieffects();
extern ImGuiContext* g_ImGuiContext;

Initialization::Initialization()
    : scene(nullptr), lightManager(nullptr), m_Window(nullptr), m_Renderer(nullptr) {
    try {
        m_Window = createWindow();

        cameraInit();
        init();

        g_ImGuiContext = ImGui::GetCurrentContext();
        if (g_ImGuiContext) {
            imguieffects();
        } else {
            std::cerr << "ImGui context is not set. Cannot call..." << std::endl;
        }

        PROFILE_SCOPE("Shader Manager Init");
        ShaderManager &shaderManager = ShaderManager::Instance();
        shaderManager.Initialize();

        glfwSetWindowUserPointer(m_Window, this);

        {
            PROFILE_SCOPE("SCENE");
            scene = std::make_shared<Scene>(shaderProgram, lightShader, finalPassShader, m_MainCamera, lightManager);
        }

        {
            PROFILE_SCOPE("RENDERER");
            m_Renderer = std::make_shared<Renderer>(scene, m_MainCamera, m_Window);
        }
    } catch (const std::exception &e) {
        std::cerr << "[Init] Exception caught in constructor: " << e.what()
                << " in thread " << std::this_thread::get_id() << std::endl;
        throw;
    }
}

Initialization::~Initialization(){
    const char* ini_path = ImGui::GetIO().IniFilename;
    if (ini_path) {
      //  std::cout << "[ImGui] Attempting to save settings to: " << ini_path << "\n";
        if (std::filesystem::exists(ini_path)) {
            auto ftime = std::filesystem::last_write_time(ini_path);
            auto system_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
      //      std::cout << " - Existing file last modified: "
      //                << std::chrono::system_clock::to_time_t(system_time) << "\n";
        }
    }

    ImGui::SaveIniSettingsToDisk(ini_path);
   // std::cout << "[ImGui] Settings saved (destructor)\n";

    if (ini_path && std::filesystem::exists(ini_path)) {
      //  std::cout << " - New file size: "
      //            << std::filesystem::file_size(ini_path) << " bytes\n";

        auto ftime = std::filesystem::last_write_time(ini_path);
        auto system_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

    //    std::cout << " - New last modified: " << std::chrono::system_clock::to_time_t(system_time) << "\n";
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (ini_path) {
    //    std::cout << "[ImGui] Post-destruction file status: "
    //              << (std::filesystem::exists(ini_path) ? "Exists" : "Missing")  << "\n";
    }
}

GLFWwindow* Initialization::createWindow() {
    {
        PROFILE_SCOPE("GLFW Initialization");
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        PROFILE_SCOPE("GLFW Window Creation");
        GLFWwindow* window = glfwCreateWindow(1280, 720, "idk", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(0);

    {
        PROFILE_SCOPE("Load OpenGL with GLAD");
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    {
        PROFILE_SCOPE("Store Main Thread ID");
        mainThreadId = std::this_thread::get_id();
        std::cout << "[main] Main thread ID: " << mainThreadId << "." << std::endl;
    }

    return window;
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
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

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


    io.IniFilename = SOURCE_DIR "/src/imgui.ini";
   // std::cout << "[ImGui] Configuring settings file: " << io.IniFilename << "\n";

    if (std::filesystem::exists(io.IniFilename)) {
    //    std::cout << "[ImGui] Loading existing settings file:\n";
    //    std::cout << " - Size: " << std::filesystem::file_size(io.IniFilename) << " bytes\n";

        // Convert last write time to system_clock::time_point
        auto ftime = std::filesystem::last_write_time(io.IniFilename);
        auto system_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

     //   std::cout << " - Last modified: " << std::chrono::system_clock::to_time_t(system_time) << "\n";

     //   std::cout << "[ImGui] Settings loaded successfully\n";
    } else {
        std::cout << "[ImGui] No existing settings file found\n";
    }

    if (std::filesystem::exists(io.IniFilename)) {
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
        if (io.Fonts->IsBuilt()) {
            ImGui_ImplOpenGL3_CreateFontsTexture();
        }
    }
}

bool Initialization::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Initialization::init()
{
  //  std::cout << "[Initialization] Starting Initialization in thread "
  //        << std::this_thread::get_id() << std::endl;

    const fs::path engineDir = SOURCE_DIR;
    if (!fs::exists(engineDir))
    {
        std::cerr << "[Initialization] Engine directory does not exist: " << engineDir << std::endl;
        throw std::runtime_error("Engine directory does not exist.");
    }
    current_path(engineDir);

    initializeImGui(m_Window);

    shaderProgram = std::make_shared<Shader>(
               SOURCE_DIR "/src/shaders/basic.vert",
               SOURCE_DIR "/src/shaders/basic.frag"
           );

    lightShader = std::make_shared<Shader>(
       SOURCE_DIR "/src/shaders/lightShader.vert",
       SOURCE_DIR "/src/shaders/lightShader.frag"
   );

    finalPassShader = std::make_shared<Shader>(
        SOURCE_DIR "/src/shaders/finalPass.vert",
        SOURCE_DIR "/src/shaders/finalPass.frag"
    );

    if (!shaderProgram || !lightShader || !finalPassShader) {
        throw std::runtime_error("Failed to load one or more shaders");
    }

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

 //   std::cout << "[INIT] Light manager: " << (lightManager ? "valid" : "null") << std::endl;
//    std::cout << "[INIT] Camera: " << (m_MainCamera ? "valid" : "null") << std::endl;
}

void Initialization::runMainLoop() const {
    while (!ShouldClose()) {
        if (!glfwGetCurrentContext()) {
            std::cerr << "No OpenGL context current!" << std::endl;
            break;
        }
        if (m_Renderer != nullptr){
            m_Renderer->render();
        }else{
            std::cerr << "m_Renderer is null\n";
        }
    }
}

