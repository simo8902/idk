//
// Created by Simeon on 4/27/2024.
//

#include "Initialization.h"
#include "CameraManager.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <LightManager.h>

#define STB_IMAGE_IMPLEMENTATION
#include <IconsFontAwesome6Brands.h>
#include "Shader.h"

#include "AssetManager.h"
#include "stb_image.h"

Initialization::Initialization() {
    AssetManager& assetManager = AssetManager::getInstance();

    NotImportantForNow();
    try {
        std::filesystem::path engineDir = normalizePath(SOURCE_DIR);
        std::filesystem::current_path(engineDir);

        std::cerr << "[Initialization.h] Current Working Directory: " << std::filesystem::current_path() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to set working directory: " << e.what() << std::endl;
    }


   // std::string sourceDir = assetManager.getSourcePath();
    assetManager.compileShaders();

    std::shared_ptr<Shader> shaderProgram = assetManager.getShader("basic");
    if (!shaderProgram) {
        std::cerr << "Failed to retrieve 'basic' shader from AssetManager." << std::endl;
        return;
    }

    std::shared_ptr<Shader> wireframeShader = assetManager.getShader("wireframe");
    if (!wireframeShader) {
        std::cerr << "Failed to retrieve 'wireframe' shader from AssetManager." << std::endl;
        // Decide whether to continue without wireframe shader or exit
    }

    std::shared_ptr<Shader> skyShaderProgram = assetManager.getShader("sky");
    if (!skyShaderProgram) {
        std::cerr << "Failed to retrieve 'sky' shader from AssetManager." << std::endl;
        // Decide whether to continue without sky shader or exit
    }

    if (!shaderProgram || !wireframeShader || !skyShaderProgram) {
        std::cerr << "Failed to retrieve shaders from AssetManager." << std::endl;
        return;
    }

    scene = new Scene(shaderProgram, wireframeShader, skyShaderProgram, m_MainCamera, skyboxTexture);
    std::cout << "[Initialization] Predefined shaders initialized and added to AssetManager with UUIDs." << std::endl;

    NotImportantForNow2();
}





void Initialization::NotImportantForNow2() {

    lightManager = std::make_shared<LightManager>();

    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    const auto& directionalLight = std::make_shared<DirectionalLight>(
        "Main Light",
        lightDirection,
        glm::vec3(0.1f, 0.1f, 0.1f),  // Ambient
        glm::vec3(1.0f, 1.0f, 1.0f),  // Diffuse
        glm::vec3(1.0f, 1.0f, 1.0f)   // Specular
    );

    directionalLight->setDirection(glm::vec3(0.0f, 50.0f, -30.0f));
    directionalLight->updateDirectionFromRotation();

    lightManager->addDirectionalLight(directionalLight);
    scene->addDirectionalLight(directionalLight);
    m_Renderer = std::make_shared<Renderer>(scene, m_MainCamera, lightManager, m_Window);


    if (directionalLight) {
        std::cout << "Directional light initialized successfully: " << directionalLight->getName() << std::endl;
    } else {
        std::cout << "Failed to initialize directional light." << std::endl;
    }

    initializeImGui(m_Window);
    initImGuiStyle();
    projectExplorer = std::make_unique<ProjectExplorer>();
}

void Initialization::NotImportantForNow() {

    cameraInit();

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

    std::vector<std::string> faces {
        SOURCE_DIR "/src/data/skybox/skybox1/pink_left_2.png",
        SOURCE_DIR "/src/data/skybox/skybox1/pink_right_3.png",
        SOURCE_DIR "/src/data/skybox/skybox1/pink_top_4.png",
        SOURCE_DIR "/src/data/skybox/skybox1/pink_bottom_5.png",
        SOURCE_DIR "/src/data/skybox/skybox1/pink_front_0.png",
        SOURCE_DIR "/src/data/skybox/skybox1/pink_back_1.png"
    };

    skyboxTexture = loadCubemap(faces);

}

Initialization::~Initialization(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_Window);
    glfwTerminate();
    shaderProgram->SaveShaderUUIDMap("shader_uuid_map.json");
}


GLuint Initialization::loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID);
            return 0;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}

std::shared_ptr<LightManager> Initialization::getLightManager() const {
    return lightManager;
}

Shader * Initialization::getShader() const {
    return shaderProgram;
}

Scene * Initialization::getScene() const {
    return scene;
}
Shader * Initialization::getWireframeShader() const {
    return wireframe;
}

const std::shared_ptr<Camera>& Initialization::getMainCamera() {
    return m_MainCamera;
}

GLFWwindow * Initialization::getWindow() const {
    return m_Window;
}

void Initialization::cameraInit() {
    glm::vec3 position = glm::vec3(0.0f, 3.0f, -7.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float moveSpeed = 3.0f;
    float mouseSensitivity = 0.3f;
    float fov = 70.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    const std::string name = "Main Camera";
    m_MainCamera = std::make_shared<Camera>(name, position, forward, up, yaw, pitch, moveSpeed, mouseSensitivity, fov, nearPlane, farPlane);

    m_ActiveCamera = m_MainCamera;
}


void Initialization::errorCallback(int error, const char *description) {
    std::cerr << "Error: " << error << " " << description << std::endl;
}

void Initialization::initImGuiStyle() {
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(0, 0);

    // Customize Colors
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

    style.WindowMenuButtonPosition = ImGuiDir_None;

    style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];
}
void Initialization::initializeImGui(GLFWwindow *window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.IniFilename = SOURCE_DIR "/src/imgui.ini";

    io.Fonts->Clear();

    const char* fontPath = SOURCE_DIR "/CascadiaCode.ttf";
    ImFont* primaryFont = io.Fonts->AddFontFromFileTTF(fontPath, 17.0f);
    if (primaryFont) {
        std::cout << "Successfully loaded primary font: " << fontPath << std::endl;
    } else {
        std::cerr << "Failed to load primary font: " << fontPath << std::endl;
    }

    // Define FontAwesome icon ranges (verify these ranges with your FontAwesome version)
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;

    const char* fontAwesomePath = SOURCE_DIR "/src/data/fonts/Font Awesome 6 Free-Solid-900.otf";
    ImFont* fontAwesome = io.Fonts->AddFontFromFileTTF(fontAwesomePath, 16.0f, &config, icons_ranges);
    if (fontAwesome) {
        std::cout << "Successfully loaded FontAwesome: " << fontAwesomePath << std::endl;
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

GLFWwindow *Initialization::createGLFWWindow(int width, int height) {
    GLFWwindow *window = glfwCreateWindow(width, height, "LupusFire_core", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

bool Initialization::initializeGLFW()
{
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    return true;
}

bool Initialization::initializeOpenGL(){
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return false;
    }
    return true;
}

bool Initialization::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Initialization::runMainLoop() const {
    while (!ShouldClose()) {
        if (m_Renderer != nullptr){
            m_Renderer->render();
        }else{
        //    std::cerr << "m_Renderer is null\n";
        }
    }
}
