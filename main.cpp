#include "src/editor/windows/Renderer.h"

std::string glmVersionString() {
    return std::to_string(GLM_VERSION_MAJOR) + "." +
           std::to_string(GLM_VERSION_MINOR) + "." +
           std::to_string(GLM_VERSION_PATCH);
}

int main() {
    Renderer renderer(1280, 720, "LupusFire");

    //MAIN LOOP
    while (!renderer.ShouldClose()) {
        renderer.render();
    }

    printf("OpenGL %s, GLSL %s, GLM %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION), glmVersionString().c_str());

    return 0;
}