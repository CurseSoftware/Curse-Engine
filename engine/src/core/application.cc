#include <iostream>
#include "core/application.h"
#include "core/events.h"

namespace gravity {

namespace core { 
Application* Application::instance = nullptr;
using namespace logger;

/// @brief Startup behavior for the application. This starts up all necessary subsystems as well.
Application* Application::startup(const std::string& name, u32 width, u32 height) noexcept {
    logger::Logger::startup();
    InputHandler::startup();
    EventHandler::startup();
    platform::Platform::startup(name, width, height);

    // Window wnd = Window::create(width, height, name).unwrap();
    // wnd.show();
    // while (!wnd.should_close()) {

    // }

    logger::Logger::get()->debug("Application created.");
    if (!Application::instance) {
        Application::instance = new Application();
        return Application::instance;
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
    EventHandler::shutdown();
    logger::Logger::shutdown();
}

/// @brief Run the application
void Application::run() {
}

/// @brief Application constructor
Application::Application() noexcept {
}

}
}

