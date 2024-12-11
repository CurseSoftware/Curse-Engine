#include <iostream>
#include "core/application.h"

namespace gravity {

namespace core { 
Application* Application::instance = nullptr;

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

void Application::shutdown() noexcept {
    // Shutdown in reverse order of the startup
    logger::Logger::get()->debug("Shutting down application");
    platform::Platform::shutdown();
    logger::Logger::shutdown();
}

result::Result<int, ApplicationError> Application::register_window() noexcept {
    return result::Err(ApplicationError::WINDOW_REGISTER);
}

Application::Application() noexcept {
}

}
}

