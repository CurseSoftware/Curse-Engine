#include <iostream>
#include "core/application.h"
#include "core/events.h"

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
    
    EventHandler::get()->register_event(EventCode::APPLICATION_QUIT, nullptr, Application::on_event);
    EventHandler::get()->register_event(EventCode::KEY_PRESSED, nullptr, Application::on_key);
    EventHandler::get()->register_event(EventCode::KEY_RELEASED, nullptr, Application::on_key);
    EventHandler::get()->register_event(EventCode::MOUSE_MOVE, nullptr, Application::on_mouse_move);
    EventHandler::get()->register_event(EventCode::RESIZED, nullptr, Application::on_resize);

    platform::Platform::startup(name, width, height);

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
}

/// @brief Run the application
void Application::run() {
    auto inst = get();
    inst->state.is_running = true;
    
    logger::Logger::get()->info("Running application.");
    while (inst->state.is_running == true) {
        Platform::get()->pump_messages();
    }
}

/// @brief Application constructor
Application::Application() noexcept {}

/// @brief Callback function to handle events
/// @param code Event Code
/// @param sender Who sent the event
/// @param listener Who is listening
/// @param data The data from the incoming event
/// @return `true` if we handle the event. `false` otherwise
bool Application::on_event(EventCode code, void* sender, void* listener, EventData data) {
    switch (code) {
        case EventCode::APPLICATION_QUIT: {
            logger::Logger::get()->info("APPLICATION_QUIT code received. Shutting down application");
            Application::get()->state.is_running = false;
            return true;
        } break;
        default: 
            break;
    }

    return false;
}

/// @brief Callback function to handle key events for the application
/// @param code Event Code
/// @param sender Who sent the event
/// @param listener Who is listening
/// @param data The data from the incoming event
/// @return `true` if we handle the event. `false` otherwise
bool Application::on_key(EventCode code, void* sender, void* listener, EventData data) {
    switch (code) {
        case EventCode::KEY_PRESSED: {
            u16 keycode = data.u16[0];
            if (keycode == Keys::KEY_ESCAPE) {
                EventHandler::get()->fire_event(EventCode::APPLICATION_QUIT, nullptr, {});
                return true;
            }

            logger::Logger::get()->debug("'%c' key pressed.", keycode);
        }
        case EventCode::KEY_RELEASED: {
            u16 keycode = data.u16[0];
            logger::Logger::get()->debug("'%c' key released.", keycode);
        }
        default:
            break;
    }

    return true;
}

/// @brief Callback function to handle window resizing events
/// @param code Event Code
/// @param sender Who sent the event
/// @param listener Who is listening
/// @param data The data from the incoming event
/// @return `true` if we handle the event. `false` otherwise
bool Application::on_resize(EventCode code, void* sender, void* listener, EventData data) {
    return true;
}

/// @brief Callback function to handle mouse movement events
/// @param code Event Code
/// @param sender Who sent the event
/// @param listener Who is listening
/// @param data The data from the incoming event
/// @return `true` if we handle the event. `false` otherwise
bool Application::on_mouse_move(EventCode code, void* sender, void* listener, EventData data) {
    switch (code) {
        case EventCode::MOUSE_MOVE: {
            logger::Logger::get()->debug("x: %i, y: %i", data.u16[0], data.u16[1]);
            return true;
        }
        default:
            break;
    }
    return false;
}

} // core namespace
} // gravity namespace