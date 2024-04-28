#include "src/Initialization.h"

int main() {
    std::shared_ptr<Initialization> init = std::make_shared<Initialization>();
    init->runMainLoop();

    Renderer renderer(init->getShaderProgram(),
                init->getWireFrameProgram(),
                init->getMainCamera(),
                init->getWindow());

    return 0;
}
