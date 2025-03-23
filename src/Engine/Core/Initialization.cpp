//
// Created by Simeon on 4/27/2024.
//

#include "Initialization.h"

#include <IconsFontAwesome6Brands.h>

#include "libData.h"
#include "ShaderManager.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define LIBDATA_API
extern "C" LIBDATA_API void imguieffects();
extern ImGuiContext* g_ImGuiContext;

namespace fs = std::filesystem;


Initialization::Initialization()
    : m_Window(nullptr), m_Renderer(nullptr), scene(nullptr)
{
    try {
       // std::cerr << "Initialization::Initialization()" << std::endl;

        const fs::path engineDir = SOURCE_DIR;
        if (!fs::exists(engineDir))
        {
            IDK_ASSERT(false, "Engine's dir doesn't exist.");
        }
        current_path(engineDir);

        m_Window = createWindow();

        cameraInit();
        initializeImGui(m_Window);

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

        // std::shared_ptr<DeferredRenderer> deferredRenderer = std::make_shared<DeferredRenderer>(scene, m_MainCamera, m_Window, "Deferred");
        // std::shared_ptr<ForwardRenderer> forwardRenderer = std::make_shared<ForwardRenderer>(scene, m_MainCamera, m_Window, "Forward");

        scene = std::make_shared<Scene>(m_MainCamera);
        m_Renderer = std::make_shared<Renderer>(scene, m_MainCamera, m_Window, "deferred");
    }catch (const std::exception &e) {
        std::cerr << "[Init] Exception caught in constructor: " << e.what()
                << " in thread " << std::this_thread::get_id() << std::endl;

        IDK_ASSERT(false, e.what());
    }
}

Initialization::~Initialization(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

GLFWwindow* Initialization::createWindow() {
    {
        PROFILE_SCOPE("GLFW Initialization");
        if (!glfwInit()) {
            IDK_ASSERT(false, "GLFW Initialization");
        }
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    PROFILE_SCOPE("GLFW Window Creation");
    GLFWwindow* window = nullptr;
    window = glfwCreateWindow(1280, 720, "idk", nullptr, nullptr);
    if (!window) {
        IDK_ASSERT(false, "GLFW Creation Failed.");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    {
        PROFILE_SCOPE("Load OpenGL with GLAD");
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            IDK_ASSERT(false, "Failed to init GLAD.");
        }
    }

    {
        PROFILE_SCOPE("Store Main Thread ID");
        mainThreadId = std::this_thread::get_id();
        std::cout << "[main] Main thread ID: " << mainThreadId << "." << std::endl;
    }

    return window;
}

GLFWwindow * Initialization::getWindow() const {
    return m_Window;
}


void Initialization::initializeImGui(GLFWwindow *window) const {
    glfwMakeContextCurrent(window);

    IDK_ASSERT(ImGui::GetCurrentContext() == nullptr, "Imgui is already initialized.");

    if (ImGui::GetCurrentContext()) {
        std::cerr << "ImGui is already initialized." << std::endl;
        return;
    }

    ImGui::CreateContext();
    IDK_ASSERT(ImGui::GetCurrentContext() != nullptr, "Failed to create Imgui context.");

  //  g_ImGuiContext = ImGui::GetCurrentContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    IMGUI_CHECKVERSION();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

  //  imguieffects();

    io.Fonts->Clear();

    const auto &fontPath = SOURCE_DIR "/src/data/fonts/CascadiaCode-Bold.ttf";
    const ImFont* primaryFont = io.Fonts->AddFontFromFileTTF(fontPath, 17.0f);
    IDK_ASSERT(primaryFont, "Failed to load primary font.");

    /*
    if (primaryFont) {
       // std::cout << "Successfully loaded primary font: " << fontPath << std::endl;
    } else {
        std::cerr << "Failed to load primary font: " << fontPath << std::endl;
    }*/

    static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;

    const auto fontAwesomePath = SOURCE_DIR "/src/data/fonts/Font Awesome 6 Free-Solid-900.otf";
    const ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF(fontAwesomePath, 17.0f, &config, icons_ranges);
    IDK_ASSERT(fontAwesome, "Failed to load FontAwesome font.");

    /*
    if (fontAwesome) {
       // std::cout << "Successfully loaded FontAwesome: " << fontAwesomePath << std::endl;
    } else {
        std::cerr << "Failed to load FontAwesome: " << fontAwesomePath << std::endl;
    }*/

    io.Fonts->Build();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    io.IniFilename = SOURCE_DIR "/src/imgui.ini";

   // std::cout << "[ImGui] Configuring settings file: " << io.IniFilename << "\n";

    if (std::filesystem::exists(io.IniFilename)) {
    //    std::cout << "[ImGui] Loading existing settings file:\n";
    //    std::cout << " - Size: " << std::filesystem::file_size(io.IniFilename) << " bytes\n";

        // Convert last write time to system_clock::time_point
      //  auto ftime = std::filesystem::last_write_time(io.IniFilename);
      //  auto system_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

     //   std::cout << " - Last modified: " << std::chrono::system_clock::to_time_t(system_time) << "\n";

     //   std::cout << "[ImGui] Settings loaded successfully\n";
    } else {
        //std::cout << "[ImGui] No existing settings file found\n";
    }

    if (std::filesystem::exists(io.IniFilename)) {
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
        IDK_ASSERT(io.Fonts->IsBuilt(), "Failed to build fonts after loading settings.");
        ImGui_ImplOpenGL3_CreateFontsTexture();
    }else
    {
        std::cout << "[ImGui] No existing settings file found\n";
    }
}

bool Initialization::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Initialization::runMainLoop() const {
    while (!ShouldClose()) {
        if (m_Renderer != nullptr){
            m_Renderer->render();
        }
    }
}

