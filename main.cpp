#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <gtc/type_ptr.hpp>

#include "src/Shader.h"
#include "src/Scene.h"

#include "src/Components/Transform.h"
#include "src/Components/BoundingBox.h"

//Resharper Disable All

Shader* shaderProgram;
Scene* globalScene;

Shader* createShaderProgram() {
    std::string vertexShaderSource = shaderProgram->readShaderFile(SOURCE_DIR "/shaders/basic.vert");
    std::string fragmentShaderSource = shaderProgram->readShaderFile(SOURCE_DIR "/shaders/basic.frag");

    shaderProgram = new Shader(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

    GLuint vertexShaderID = shaderProgram->compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    GLuint fragShaderID = shaderProgram->compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    if (vertexShaderID == 0) {
        std::cerr << "Failed to compile vertex shader" << std::endl;
        return nullptr;
    }

    if (fragShaderID == 0) {
        std::cerr << "Failed to compile fragment shader" << std::endl;
        return nullptr;
    }

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderID);
    glAttachShader(programId, fragShaderID);
    glLinkProgram(programId);

    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return nullptr;
    }

    shaderProgram->setProgramId(programId);

    return shaderProgram;
}

std::string glmVersionString() {
    return std::to_string(GLM_VERSION_MAJOR) + "." +
           std::to_string(GLM_VERSION_MINOR) + "." +
           std::to_string(GLM_VERSION_PATCH);

}

void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << error << " " << description << std::endl;
}

static float fov = 90.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

    for (auto object : globalScene->getGameObjects()) {
        ImGui::Text(object->getName().c_str());
        // No TreeNode at this level
    }

    ImGui::End();
}

GLFWwindow* createGLFWWindow() {
    GLFWwindow* window = glfwCreateWindow(800, 600, "Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Cleanup if window creation fails
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

bool initializeGLFW() {
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    return true;
}

bool initializeOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return false;
    }
    return true;
}


void renderProjectExplorer() {
    ImGui::Begin("Project Explorer");
    ImGui::Text("Project Explorer");
    ImGui::End();
}

void initializeImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;
}


int main() {
    if (!initializeGLFW())
        return -1;

    GLFWwindow *window = createGLFWWindow();
    if (!window)
        return -1;

    if (!initializeOpenGL()){
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    shaderProgram = createShaderProgram();
    if(!shaderProgram){
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    initializeImGui(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int display_w, display_h;
    globalScene = new Scene(shaderProgram);

    // First Cube
    std::shared_ptr<GameObject> firstCube = std::make_shared<Cube>("Blah Cube");
    firstCube->addComponent<Transform>();
    firstCube->addComponent<BoundingBox>();
    firstCube->setColor(glm::vec3(1.0f, 1.0f, 0.0f));

    //TODO
    BoundingBox* boundingBox = firstCube->getComponent<BoundingBox>();

    Transform* transformComponent = firstCube->getComponent<Transform>();
    if (transformComponent) {

        transformComponent->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        transformComponent->setRotation(glm::vec3(0.0f, 0.0f, 45.0f));
        transformComponent->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // Second Cube
    std::shared_ptr<GameObject> mySecondCube = std::make_shared<Cube>("Another Blah Cube");
    mySecondCube->addComponent<Transform>();
    mySecondCube->setColor(glm::vec3(1.0f, 0.0f, 0.0f));

    Transform* secondCubeTransform = mySecondCube->getComponent<Transform>();
    if (secondCubeTransform) {
        secondCubeTransform->setPosition(glm::vec3(-3.0f, 2.0f, 0.0f));
    }

    // Third Cube
    std::shared_ptr<GameObject> thirdCube = std::make_shared<Cube>("Third Damn Blah Cube");
    thirdCube->addComponent<Transform>();
    thirdCube->setColor(glm::vec3(0.0f, 0.0f, 1.0f));

    Transform* thirdCubeTransform = thirdCube->getComponent<Transform>();
    if (thirdCubeTransform) {
        thirdCubeTransform->setPosition(glm::vec3(3.0f, 2.0f, 0.0f));
    }


    globalScene->addGameObject(firstCube);
    globalScene->addGameObject(mySecondCube);
    globalScene->addGameObject(thirdCube);

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

        glfwGetFramebufferSize(window, &display_w, &display_h);
        globalScene->calculateProjectionMatrix(display_w, display_h);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


        glPushMatrix();

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // Get the size of the window
            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            // Get the position of the mouse cursor
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Convert the mouse cursor coordinates to normalized device coordinates
            float normalizedX = (2.0f * mouseX) / windowWidth - 1.0f;
            float normalizedY = 1.0f - (2.0f * mouseY) / windowHeight;

            // Create a ray in clip coordinates
            glm::vec4 rayClip = glm::vec4(normalizedX, normalizedY, -1.0, 1.0);

            // Convert the ray to eye coordinates
            glm::vec4 rayEye = glm::inverse(globalScene->calculateProjectionMatrix(display_w, display_h)) * rayClip;
            rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

            // Convert the ray to world coordinates
            glm::vec3 rayWorld = glm::inverse(globalScene->calculateViewMatrix()) * rayEye;
            rayWorld = glm::normalize(rayWorld);
        }

        // Main menu bar
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
        renderHierarchy();


        globalScene->update();
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        globalScene->renderSceneView(display_w, display_h);


        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderInspector();
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderGameView();
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
        renderProjectExplorer();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glPopMatrix();
    }

    printf("OpenGL %s, GLSL %s, GLM %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION), glmVersionString().c_str());

    // Cleanup
    delete shaderProgram;
    delete globalScene;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}