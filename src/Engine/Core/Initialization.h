//
// Created by Simeon on 4/27/2024.
//

#ifndef CORE_INITIALIZATION_H
#define CORE_INITIALIZATION_H

#include <thread>

#include "Renderer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Scene.h"

class Initialization {
public:
    static Initialization& getInstance() {
        /*
        static std::once_flag flag;
        static Initialization* instance;
        std::call_once(flag, []() { instance = new Initialization(); });
        return *instance;*/
        static Initialization instance;
        return instance;
    }

    Initialization();
    ~Initialization();

    void runMainLoop() const;
    bool ShouldClose() const;

    void initializeImGui(GLFWwindow *window) const;

    const std::shared_ptr<Scene> & getScene() const;

    GLFWwindow* getWindow() const;

    void cameraInit();

    Initialization(Initialization const&) = delete;
    void operator=(Initialization const&) = delete;

    GLFWwindow* createWindow();

    GLuint loadCubemap(std::vector<std::string> faces);

    bool isMainThread() const {
        return std::this_thread::get_id() == mainThreadId;
    }

private:
    std::thread::id mainThreadId;

    GLuint skyboxTexture{};
    GLFWwindow* m_Window;

    std::shared_ptr<Camera> m_MainCamera;
    std::shared_ptr<Renderer> m_Renderer;
    std::shared_ptr<Scene> scene;

};

#endif