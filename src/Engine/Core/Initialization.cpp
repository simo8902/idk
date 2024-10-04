//
// Created by Simeon on 4/27/2024.
//

#include "Initialization.h"
#include "CameraManager.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <LightManager.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CameraManager cameraManager;

Initialization::Initialization() {
    cameraInit();

    if (m_MainCamera) {
        std::cout << "Main camera successfully passed to Renderer.\n";
    } else {
        std::cerr << "Failed to pass main camera to Renderer!\n";
    }

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

    std::shared_ptr<Shader> shaderProgram = std::make_shared<Shader>(SOURCE_DIR "/shaders/basicVertex.glsl", SOURCE_DIR "/shaders/basicFragment.glsl");
    std::shared_ptr<Shader> wireframe = std::make_shared<Shader>(SOURCE_DIR "/shaders/wireframeVert.glsl", SOURCE_DIR "/shaders/wireframeFrag.glsl");
    std::shared_ptr<Shader> skyShaderProgram = std::make_shared<Shader>(SOURCE_DIR "/shaders/vertexsky.glsl", SOURCE_DIR "/shaders/fragmentsky.glsl");

    scene = new Scene(shaderProgram, wireframe,skyShaderProgram, m_MainCamera, skyboxTexture);

    lightManager = std::make_shared<LightManager>();

    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    const auto & directionalLight = std::make_shared<DirectionalLight>(
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

    if (directionalLight) {
        std::cout << "Directional light initialized successfully: " << directionalLight->getName() << std::endl;
    } else {
        std::cout << "Failed to initialize directional light." << std::endl;
    }

    cameraManager.addCamera(m_MainCamera);


    m_Renderer = std::make_shared<Renderer>(scene,m_MainCamera, lightManager, m_Window);

    initializeImGui(m_Window);
    initImGuiStyle();
}

Initialization::~Initialization(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_Window);
    glfwTerminate();
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

void Initialization::initImGuiStyle(){
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
    if (io.Fonts->AddFontFromFileTTF(fontPath, 17.0f)) {
    } else {
        std::cerr << "Failed to load font: " << strerror(errno) << std::endl;
        return;
    }

    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    const char* fontAwesomePath = SOURCE_DIR "/src/data/fonts/Font Awesome 6 Free-Solid-900.otf";
    if (io.Fonts->AddFontFromFileTTF(fontAwesomePath, 16.0f, &config, icons_ranges)) {
    } else {
        std::cerr << "Failed to load Font Awesome: " << strerror(errno) << std::endl;
        return;
    }

    io.Fonts->Build();

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
            std::cerr << "m_Renderer is null\n";
        }
    }
}
