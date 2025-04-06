//
// Created by SIMEON on 4/5/2025.
//

#include "EngineSystems.h"

#include <IconsFontAwesome6Brands.h>

#include "ECScheduler.h"

#define ENABLE_MEMORY_TRACKING
#include "libData.h"
#include "MainAllocator.h"

#include "Renderer.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "ShaderManager.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define LIBDATA_API
extern "C" LIBDATA_API void imguieffects();
extern ImGuiContext* g_ImGuiContext;

namespace IDK
{
    const std::string EngineSystems::Config::defaultTitle = "idkCoreEngine";
    const std::string EngineSystems::Config::defaultRendererType = "deferred";
    const std::filesystem::path EngineSystems::Config::defaultFontPath = SOURCE_DIR "/src/data/fonts/CascadiaCode-Bold.ttf";
    const std::filesystem::path EngineSystems::Config::defaultIconFontPath = SOURCE_DIR "/src/data/fonts/Font Awesome 6 Free-Solid-900.otf";

    struct EngineSystems::Impl
    {
        Config config;

        // Core sys
        GLFWwindow* m_Window = nullptr;
        std::shared_ptr<IDK::Graphics::Camera> m_MainCamera;
        std::shared_ptr<Scene> m_Scene;
        std::shared_ptr<Renderer> m_Renderer;
        std::unique_ptr<ECScheduler> scheduler;
        std::thread::id mainThreadId;

        // subsystem states
        bool imguiInitialized = false;
        ImGuiContext* imguiContext = nullptr;

        explicit Impl(const Config& cfg) :
            config(cfg)
        {
            initializeGLFW();
            initializeCoreComponents();
            initializeImGui();
            initializeRenderer();
            initializeScheduler();
        }

        ~Impl() {
            shutdownAll();
        }

        void initializeGLFW()
        {
            PROFILE_SCOPE("GLFW Initialization");
            if (!glfwInit()) {
                IDK_ASSERT(false, "GLFW Initialization");
            }

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            {
                PROFILE_SCOPE("GLFW Window Creation");
                m_Window = glfwCreateWindow(config.window.width, config.window.height, config.window.title.c_str(), nullptr, nullptr);

                if (!m_Window) {
                    IDK_ASSERT(false, "GLFW Creation Failed.");
                }

                TRACK_ALLOC(m_Window, "GLFWwindow creation");
            }

            IDK::GLFWMemoryTracker::TrackWindowCreation(m_Window);
            glfwMakeContextCurrent(m_Window);
            glfwSwapInterval(1);

            {
                PROFILE_SCOPE("Load OpenGL with GLAD");
                if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
                    IDK_ASSERT(false, "Failed to init GLAD.");
            }

            {
                PROFILE_SCOPE("Store Main Thread ID");
                mainThreadId = std::this_thread::get_id();

                std::cout << "[main] Main thread ID: " << mainThreadId << std::endl;
            }
        }

        void initializeCoreComponents()
        {
            const std::filesystem::path engineDir = SOURCE_DIR;
            if (!fs::exists(engineDir))
                IDK_ASSERT(false, "Engine's dir doesn't exist.");

            current_path(engineDir);

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

            m_MainCamera = std::make_shared<IDK::Graphics::Camera>(name, position, forward, up, yaw, pitch, moveSpeed, mouseSensitivity, fov, nearPlane, farPlane);
            m_Scene = std::make_shared<IDK::Scene>(m_MainCamera);

            TRACK_ALLOC(m_MainCamera, "Main Camera");
            TRACK_ALLOC(m_Scene, "Scene");
        }

        void initializeImGui()
        {
            glfwMakeContextCurrent(m_Window);
            IDK_ASSERT(ImGui::GetCurrentContext() == nullptr, "Imgui is already initialized.");

            if (ImGui::GetCurrentContext())
                IDK_ASSERT(false, "ImGui is already initialized");

            ImGui::CreateContext();
            IDK_ASSERT(ImGui::GetCurrentContext() != nullptr, "Failed to create Imgui context.");

            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

            IMGUI_CHECKVERSION();
            ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
            ImGui_ImplOpenGL3_Init("#version 460");

            g_ImGuiContext = ImGui::GetCurrentContext();
            if (g_ImGuiContext) {
                imguieffects();
            } else {
                std::cerr << "ImGui context is not set. Cannot call..." << std::endl;
            }

            io.Fonts->Clear();
            const auto &fontPath = config.graphics.fontPath;
            const ImFont* primaryFont = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 17.0f);
            IDK_ASSERT(primaryFont, "Failed to load primary font.");

            static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            ImFontConfig imfntcfg;
            imfntcfg.MergeMode = true;
            imfntcfg.PixelSnapH = true;
            const auto iconPath = config.graphics.iconFontPath;
            const ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF(iconPath.string().c_str(),\
                17.0f, &imfntcfg, icons_ranges);
            IDK_ASSERT(fontAwesome, "Failed to load FontAwesome font.");
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
                IDK_ASSERT(false, "[ImGui] No existing settings file found.");

        }
        void initializeRenderer()
        {
            PROFILE_SCOPE("Renderer Initialization");
            ShaderManager& shaderManager = ShaderManager::Instance();
            shaderManager.Initialize();

            m_Renderer = std::make_shared<Renderer>(
                m_Scene,
                m_MainCamera,
                m_Window,
                config.graphics.rendererType
            );

            TRACK_ALLOC(m_Renderer, "Renderer");
        }

        void initializeScheduler()
        {
            PROFILE_SCOPE("Scheduler Initialization");
            scheduler = std::make_unique<ECScheduler>();

            /*
            scheduler.addSystem([this]() { m_Renderer->render(); },
                    ECScheduler::ExecutionPolicy::MainThread,
                    "Rendering");*/

            scheduler->addSystem<RenderSystem>(
                ECScheduler::ExecutionPolicy::MainThread,
                &RenderSystem::render,
                m_Renderer
            );

            // TRACK_ALLOC(scheduler, "Scheduler");
        }

        void shutdownAll()
        {
            if (scheduler)
            {
                scheduler->shutdown();
                scheduler.reset();
            }

            if (m_Renderer)
            {
                m_Renderer.reset();
                glFinish();
                ShaderManager::Instance().Shutdown();
                UNTRACK_ALLOC(m_Renderer, "Renderer");
            }

            if (m_Scene)
            {
                m_Scene.reset();
                UNTRACK_ALLOC(m_Scene, "Scene");
            }

            if (m_MainCamera)
            {
                m_MainCamera.reset();
                UNTRACK_ALLOC(m_MainCamera, "Main Camera");
            }

            if (g_ImGuiContext && ImGui::GetCurrentContext() == g_ImGuiContext) {
                if (m_Window) {
                    glfwMakeContextCurrent(m_Window);
                }

                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext();
                g_ImGuiContext = nullptr;
            }

            if (m_Window) {
                UNTRACK_ALLOC(m_Window, "GLFWwindow destruction");
                glfwSetWindowUserPointer(m_Window, nullptr);
                glfwSetWindowCloseCallback(m_Window, nullptr);
                glfwSetKeyCallback(m_Window, nullptr);
                glfwSetCursorPosCallback(m_Window, nullptr);

                IDK::GLFWMemoryTracker::SafeWindowDestruction(m_Window);
                m_Window = nullptr;
            }

            glfwTerminate();

            mainThreadId = std::thread().get_id();
            if (mainThreadId == std::thread().get_id()) {
                // std::cout << "mainThreadId is invalid (cleared)" << std::endl;
            } else {
                std::cerr << "mainThreadId is still valid" << std::endl;
            }

            std::cerr << "[Initialization] Destructor done" << std::endl;
        }
    };

    EngineSystems::EngineSystems(const Config& config)
        : pImpl(std::make_unique<Impl>(config))
    {
        pImpl->config = config;
        pImpl->mainThreadId = std::this_thread::get_id();
    }

    EngineSystems::~EngineSystems() = default;

    GLFWwindow* EngineSystems::getWindow() const { return pImpl->m_Window; }
    std::shared_ptr<IDK::Graphics::Camera> EngineSystems::getMainCamera() const { return pImpl->m_MainCamera; }
    std::shared_ptr<Renderer> EngineSystems::getRenderer() const { return pImpl->m_Renderer; }
    std::shared_ptr<Scene> EngineSystems::getScene() const { return pImpl->m_Scene; }
    ECScheduler& EngineSystems::getScheduler() const { return *pImpl->scheduler; }

    void EngineSystems::runMainLoop() const
    {
        try
        {
            bool running = true;
            while (running)
            {
                if (glfwWindowShouldClose(pImpl->m_Window)){
                    running = false;
                }else
                {
                    if (pImpl->m_Renderer && pImpl->m_Window)
                    {
                        pImpl->m_Renderer->render();
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "E caught in runMainLoop: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown e caught in runMainLoop" << std::endl;
        }
    }
}