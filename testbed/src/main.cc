#include <iostream>
// #include "gravity.h"
#include <core/application.h>



int main() noexcept {
    auto app = gravity::core::Application::create();

    app.shutdown();
    

    return EXIT_SUCCESS;
}