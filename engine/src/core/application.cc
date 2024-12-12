#include <iostream>
#include "core/application.h"

namespace gravity {

namespace core { 
Application* Application::instance = nullptr;

/// @brief Startup behavior for the application. This starts up all necessary subsystems as well.
void Application::startup() noexcept {
    logger::Logger::startup();
    platform::Platform::startup();

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
    logger::Logger::get()->debug("Shutting down application");
    platform::Platform::shutdown();
    logger::Logger::shutdown();
}

result::Result<int, ApplicationError> Application::register_window(
    std::string name,
    u32 width,
    u32 height
) noexcept {
    return result::Err(ApplicationError::WINDOW_REGISTER);
}

/// @brief Application constructor
Application::Application() noexcept {
}

}
}

