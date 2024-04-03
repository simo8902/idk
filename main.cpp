#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "GUI.h"
#include "Shader.h"
#include "Cube.h"

#include <fstream>
#include <sstream>

//resharper disable all

Shader *m_shader;
void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << error << " " << description << std::endl;
}

const GLint WIDTH = 800;
const GLint HEIGHT = 600;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
GLuint FBO;
GLuint texture_id;
GLuint RBO;
Cube* cube;

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

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

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


void Render3DScene(float display_w, float display_h, glm::mat4 projection, glm::mat4 view) {
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();

    float window_y = display_h - (pos.y + size.y);

    glViewport(pos.x, window_y, size.x, size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 model = glm::mat4(1.0f); // Identity matrix (no transformation)

    m_shader->Use();
    m_shader->setMat4("model", model);
    m_shader->setMat4("view", view);
    m_shader->setMat4("projection", projection);
    m_shader->setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 lightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    m_shader->setVec3("ambientColor", lightDir);

    cube->Draw(reinterpret_cast<Shader &>(m_shader));

    glFlush();
}
static bool showSceneView = false;

void renderSceneView(int display_w, int display_h) {
    ImGui::Begin("Scene View");
    if (ImGui::Button("Scene")) {
        showSceneView = !showSceneView;

        if (showSceneView)
        {
        //    create_framebuffer();

         //   glBindFramebuffer(GL_FRAMEBUFFER, FBO);
           // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float aspectRatio = ImGui::GetWindowSize().x / ImGui::GetWindowSize().y;

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 1.0f, 100.0f);

            glm::mat4 view = glm::lookAt(glm::vec3(1.0f, 2.0f, 3.0f),  // Camera position (eye)
                                         glm::vec3(0.0f, 0.0f, 0.0f),  // Camera target (center)
                                         glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector

            Render3DScene(display_w, display_h, projection, view);

        //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Game")) {
    }

    if (showSceneView)
    {
        ImGui::Image((void*)(intptr_t)texture_id, ImVec2(1280, HEIGHT));
    }
    ImGui::End();

}


void renderGameView() {
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


int main(int argc, char** argv) {

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

    // Read the shader source code from the files
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

    Shader shader(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

    GLint success;
    GLchar infoLog[512];

    // Check shader program linking
    glGetProgramiv(shader.getProgramId(), GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader.getProgramId(), 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return -1;  // Return -1 if linking failed
    }

    GUI gui(vertexShaderSource.c_str(), fragmentShaderSource.c_str());


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    //MAIN LOOP
    while (!glfwWindowShouldClose(window)) {

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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}