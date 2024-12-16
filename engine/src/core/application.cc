#include <iostream>
#include "core/application.h"

namespace gravity {

namespace core { 
Application* Application::instance = nullptr;
using namespace logger;

/// @brief Startup behavior for the application. This starts up all necessary subsystems as well.
void Application::startup() noexcept {
    logger::Logger::startup();
    platform::Platform::startup();
    InputHandler::startup();


    logger::Logger::get()->debug("Application created.");
    if (!Application::instance) {
        Application::instance = new Application();
    } else {
        exit(1);
    }
}

/// @brief Shutdown application and all subsystems.
void Application::shutdown() noexcept {
    // Shutdown in reverse order of the startup
    logger::Logger::get()->debug("Shutting down application...");

    InputHandler::shutdown();
    platform::Platform::shutdown();
    logger::Logger::shutdown();
}

result::Result<Window, ApplicationError> Application::register_window(
    std::string name,
    u32 width,
    u32 height
) noexcept {
    auto r_window = Window::create(width, height, name);

    r_window.match(
        [](Window& wnd) {
            Logger::get()->info("Window created successfully");
        },
        [](WindowError err) {
            Logger::get()->error("Window creation failed");
        }
    );

    return Err(ApplicationError::WINDOW_REGISTER);
}

/// @brief Application constructor
Application::Application() noexcept {
}

}
}

