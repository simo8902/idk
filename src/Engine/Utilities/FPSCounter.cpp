//
// Created by simeon on 22.01.25.
//

#include "FPSCounter.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

FPSCounter::FPSCounter() : previousTime(glfwGetTime()), frameCount(0), fps(0.0f) {}

void FPSCounter::update() {
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - previousTime >= 1.0) {
        fps = frameCount / static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;
        frameCount = 0;
    }
}

float FPSCounter::getFPS() const {
    return fps;
}
