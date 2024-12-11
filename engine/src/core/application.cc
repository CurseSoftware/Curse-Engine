#include <iostream>
#include "core/application.h"

namespace gravity {

namespace core { 

Application Application::create() noexcept {
    Application a = Application();

    logger::Logger::startup();
    platform::Platform::startup();

    logger::Logger::get()->debug("Application created.");
    return a;
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

