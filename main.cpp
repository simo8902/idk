<<<<<<< HEAD
#include "Initialization.h"

int main() {
    const auto init = std::make_shared<Initialization>();

    init->runMainLoop();

    Renderer renderer(init->getScene(),
        init->getMainCamera(),
        init->getWindow()
    );
=======
#include "src/Initialization.h"

int main() {
    std::shared_ptr<Initialization> init = std::make_shared<Initialization>();
    init->runMainLoop();

    Renderer renderer(init->getShaderProgram(),
                init->getWireFrameProgram(),
                init->getMainCamera(),
                init->getWindow());
>>>>>>> aff175ea6ae47cf074278c03587efcafbb7d96ac

    return 0;
}
