#include "Initialization.h"
static constexpr float VERSION = 0.061;

int main() {
    try {
        const Initialization init;
        init.runMainLoop();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}