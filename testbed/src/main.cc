#include <iostream>
// #include "gravity.h"
#include <core/application.h>



int main() noexcept {
    auto app = gravity::core::Application::startup(
        "Gravity",
        800,
        600
    );

    app->run();
    gravity::core::Application::shutdown();

    return EXIT_SUCCESS;
}