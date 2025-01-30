#include "Initialization.h"
#include <thread>

static constexpr float VERSION = 0.060;

std::thread::id mainThreadId;

#define GLCheckError() _GLCheckError(__FILE__, __LINE__)
void _GLCheckError(const char* file, const int &line) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error in " << file << " at line " << line << ": " << err << "\n";
    }
}

bool isMainThread() {
    return std::this_thread::get_id() == mainThreadId;
}

int main() {
    GLFWwindow* window = nullptr;
    try {
        setenv("XMODIFIERS", "@im=none", 1);

        glfwSetErrorCallback([](int error, const char* description) {
            std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
        });

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return EXIT_FAILURE;
        }

        window = glfwCreateWindow(1280, 720, "idk", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }

        mainThreadId = std::this_thread::get_id();
        std::cout << "[main] Main thread ID: " << mainThreadId << "." << std::endl;
        glfwSwapInterval(0);

        const Initialization init(window);
        init.runMainLoop();

    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
        return EXIT_FAILURE;
    }

    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}