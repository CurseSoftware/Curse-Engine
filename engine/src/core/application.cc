#include <iostream>
#include "core/application.h"
#include "core/events/events.h"

namespace gravity {

namespace core { 
using namespace logger;

// Singleton for application
Application* Application::instance = nullptr;

/// @brief Startup behavior for the application. This starts up all necessary subsystems as well.
Application* Application::startup(const std::string& name, u32 width, u32 height) noexcept {
    logger::Logger::startup();
    InputHandler::startup();
    EventHandler::startup();

    platform::Platform::startup(name, width, height);

    EventHandler::get()->register_callback(
        platform::Platform::get()->get_primary_window(),
        EventType::APPLICATION_QUIT,
        Application::on_event,
        "application",
        EventPriority::HIGH
    );
    
    EventHandler::get()->register_callback(
        platform::Platform::get()->get_primary_window(),
        EventType::KEY_PRESSED,
        Application::on_key,
        "application",
        EventPriority::NORMAL
    );
    
    EventHandler::get()->register_callback(
        platform::Platform::get()->get_primary_window(),
        EventType::KEY_RELEASED,
        Application::on_key,
        "application",
        EventPriority::NORMAL
    );

    logger::Logger::get()->debug("Application created.");
    if (!Application::instance) {
        Application::instance = new Application();
        Application::instance->state = {
            .name = name,
            .should_quit = false,
        };
        return Application::instance;
    } else {
        exit(1);
    }
}

/// @brief Shutdown application and all subsystems.
void Application::shutdown() noexcept {
    // NOTE: Shutdown in reverse order of the startup
    logger::Logger::get()->debug("Shutting down application...");
    InputHandler::shutdown();
    platform::Platform::shutdown();
    EventHandler::shutdown();
    logger::Logger::shutdown();

    std::cout << "Application shutdown successfully.\n";
}

/// @brief Run the application
void Application::run() {
    auto inst = get();
    inst->state.is_running = true;
    
    logger::Logger::get()->info("Running application.");
    while (inst->state.is_running == true) {
        Platform::get()->pump_messages();
        EventHandler::get()->poll_events();
    }
}

/// @brief Application constructor
Application::Application() noexcept {}

/// @brief Standard event handler for the application
/// @param event Incoming event to handle
/// @param context relevent context for event handling
/// @return true if handled false otherwise
bool Application::on_event(Event& event, EventContext& context) {
    switch (event.type()) {
        case EventType::APPLICATION_QUIT: {
            Application::get()->state.is_running = false;
            return true;
        } break;
        default:
            break;
    }
    return false;
}

/// @brief Standard event handler for the application
/// @param event Incoming event to handle
/// @param context relevent context for event handling
/// @return true if handled false otherwise
bool Application::on_key(Event& event, EventContext& context) {
    switch (event.type()) {
        case EventType::KEY_PRESSED: {
            KeyPressedEvent& key_event = dynamic_cast<KeyPressedEvent&>(event);
            Keys key = key_event.key();
            
            if (key == Keys::KEY_ESCAPE) {
                logger::Logger::get()->debug("ESCAPE KEY HIT QUITTING NOW");
                Application::get()->state.is_running = false;
                return true;
            }

            logger::Logger::get()->debug("KEY '%c' PRESSED", key);
            return true;
        } break;
        
        case EventType::KEY_RELEASED: {
            KeyReleasedEvent& key_event = dynamic_cast<KeyReleasedEvent&>(event);
            logger::Logger::get()->debug("KEY '%c' RELEASED", key_event.key());
            return true;
        } break;
        default:
            break;
    }
    return false;
}

} // core namespace
} // gravity namespace