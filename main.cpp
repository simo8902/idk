#include "Initialization.h"

int main() {
    const auto & init = std::make_shared<Initialization>();

    init->runMainLoop();

    const Renderer renderer(init->getScene(),
        init->getMainCamera(),
        init->getLightManager(),
        init->getWindow()
    );
}