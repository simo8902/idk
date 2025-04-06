//
// Created by SIMEON on 4/5/2025.
//

#ifndef ENGINESYSTEMS_H
#define ENGINESYSTEMS_H

#include <memory>
#include <vector>
#include <functional>
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "imgui.h"
#include <filesystem>
#include "ECScheduler.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"

namespace IDK
{
    class Renderer;
    class Scene;

    class EngineSystems {
    public:

        struct Config
        {
            struct WindowConfig
            {
                int width;
                int height;
                std::string title;
                bool vsync;
            };

            struct GraphicsConfig
            {
                std::string rendererType;
                std::filesystem::path fontPath;
                std::filesystem::path iconFontPath;
            };

            WindowConfig window;
            GraphicsConfig graphics;

            static constexpr int defaultWidth = 1280;
            static constexpr int defaultHeight = 720;
            static const std::string defaultTitle;
            static constexpr bool defaultVSync = true;
            static const std::string defaultRendererType;
            static const std::filesystem::path defaultFontPath;
            static const std::filesystem::path defaultIconFontPath;

            Config() : window{defaultWidth, defaultHeight, defaultTitle, defaultVSync},
                       graphics{defaultRendererType, defaultFontPath, defaultIconFontPath} {}
        };

        explicit EngineSystems(const Config& config = Config());
        ~EngineSystems();

        // sys access
        GLFWwindow* getWindow() const;
        std::shared_ptr<IDK::Graphics::Camera> getMainCamera() const;
        std::shared_ptr<Renderer> getRenderer() const;
        std::shared_ptr<Scene> getScene() const;
        ECScheduler& getScheduler() const;

        void runMainLoop() const;
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

#endif //ENGINESYSTEMS_H