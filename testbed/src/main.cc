#include <iostream>
// #include "gravity.h"
#include <core/application.h>



int main() noexcept {
    gravity::core::Application::startup();
    gravity::core::Application::shutdown();

    return EXIT_SUCCESS;
}