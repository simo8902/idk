#include "src/editor/windows/Renderer.h"

int main() {
    Renderer* renderer = new Renderer();

    //MAIN LOOP
    while (!renderer->ShouldClose()) {
        renderer->render();
    }

    return 0;
}