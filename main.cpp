#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Cube.h"
#include "Camera.h"

#include <fstream>
#include <sstream>

//resharper disable all

Shader *m_shader;
void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << error << " " << description << std::endl;
}

GLuint FBO;
GLuint texture_id;
GLuint RBO;
Cube* cube;

const GLint DEFAULT_WIDTH = 800;
const GLint DEFAULT_HEIGHT = 600;

GLint WIDTH = DEFAULT_WIDTH;
GLint HEIGHT = DEFAULT_HEIGHT;

static float fov = 90.0f;

glm::vec3 cameraPosition = glm::vec3(0.0f, 1.0f, -10.0f);
Camera camera(cameraPosition, // Camera position
              glm::vec3(0.0f, 0.0f, 0.0f),   // Target position (looking at the origin)
              glm::vec3(0.0f, 1.0f, 0.0f));  // Up direction

void create_framebuffer()
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
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

// Function to read shader file contents into a string
std::string ReadShaderFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint CompileShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}

void DrawGrid(float size, float step) {
    m_shader->Use();
    m_shader->setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f));

    glBegin(GL_LINES);

    for (float i = -size; i <= size; i += step) {
        // Draw lines parallel to the Z-axis (front and back)
        glVertex3f(-size, 0.0f, i);
        glVertex3f(size, 0.0f, i);

        // Draw lines parallel to the X-axis (left and right)
        glVertex3f(i, 0.0f, -size);
        glVertex3f(i, 0.0f, size);
    }

    glEnd();
}


void Render3DScene(float display_w, float display_h, glm::mat4 model, glm::mat4 projection, glm::mat4 view) {
    glViewport(0, 0, static_cast<int>(display_w), static_cast<int>(display_h));

    if (m_shader == nullptr) {
        std::cout << "m_shader is not initialized!" << std::endl;
        return;
    }

    glClear(GL_DEPTH_BUFFER_BIT);


    m_shader->Use();
    m_shader->setMat4("model", model);
    m_shader->setMat4("view", view);
    m_shader->setMat4("projection", projection);
    m_shader->setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));

    if (cube == nullptr) {
        std::cout << "cube is not initialized!" << std::endl;
        return;
    }

    cube->Draw(*m_shader);

    glFlush();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static bool framebufferInitialized = false;
static bool depthTestEnabled = false;
float aspectRatio = static_cast<float>(1920) / static_cast<float>(1280);


void renderSceneView(int display_w, int display_h) {
    ImGui::Begin("Scene View");
    float aspectRatio2 = ImGui::GetWindowSize().x / ImGui::GetWindowSize().y;

    if (!framebufferInitialized || display_w != WIDTH || display_h != HEIGHT) {
        if (display_w <= 0 || display_h <= 0) {
            // Do not render if window is minimized
            ImGui::End();
            return;
        }

        WIDTH = display_w;
        HEIGHT = display_h;
        create_framebuffer();
        framebufferInitialized = true;
    }

    if (!depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
        depthTestEnabled = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawGrid(10.0f, 1.0f);

    // Adjust aspect ratio
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio2, 1.0f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    //float angle = glfwGetTime();
    // model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

    Render3DScene(WIDTH, HEIGHT, model, projection, view);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::Image((void*)(intptr_t)texture_id, windowSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}


void renderGameView(){
    ImGui::Begin("Game View");
    ImGui::Text("Game View");
    ImGui::End();
}

void renderInspector() {
    ImGui::Begin("Inspector");
    ImGui::Text("Inspector");
    ImGui::End();
}

void renderHierarchy() {
    ImGui::Begin("Hierarchy");
    ImGui::Text("Hierarchy");
    ImGui::End();
}

void renderProjectExplorer() {
    ImGui::Begin("Project Explorer");
    ImGui::Text("Project Explorer");
    ImGui::End();
}

int main() {

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Window", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::string vertexShaderSource = ReadShaderFile(SOURCE_DIR "/shaders/basic.vert");
    std::string fragmentShaderSource = ReadShaderFile(SOURCE_DIR "/shaders/basic.frag");

    GLuint vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    if (vertexShaderID == 0) {
        std::cerr << "Failed to compile vertex shader" << std::endl;
        return -1;
    }
    GLuint fragShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());
    if (fragShaderID == 0) {
        std::cerr << "Failed to compile fragment shader" << std::endl;
        return -1;
    }

    m_shader = new Shader(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

    GLint success;
    GLchar infoLog[512];

    // Check shader program linking
    glGetProgramiv(m_shader->getProgramId(), GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_shader->getProgramId(), 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return -1;
    }

    cube = new Cube();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //MAIN LOOP
    while (!glfwWindowShouldClose(window)) {

        // Check if the window is minimized
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            glfwWaitEvents();
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Main menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Ctrl+W"))  { /* Do stuff */ }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Render panels
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderHierarchy();
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderSceneView(display_w, display_h);
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderInspector();
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderGameView();
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderProjectExplorer();

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Cleanup

    delete m_shader;
    delete cube;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}