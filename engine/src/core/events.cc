#include "core/events.h"
#include "core/defines.h"
#include "core/logger.h"


namespace gravity {
namespace core {

EventHandler* EventHandler::handler_instance = nullptr;

/// @brief Startup behavior for the event subsystem
/// @return true if successful false otherwise
bool EventHandler::startup() {
    if (handler_instance == nullptr) {
        handler_instance = new EventHandler();
    } else {
        logger::Logger::get()->error("Attempting to startup event subsystem that has already been initialized");
        return false;
    }

    return true;
}

bool EventHandler::shutdown() {
    if (handler_instance == nullptr) {
        logger::Logger::get()->warn("Attempting shutdown of event subsystem that is not initialized.");
    }

    for (usize i = 0; i < EVENT_CODE_AMOUNT; i++) {
        handler_instance->_registry[i] = {};
    }

    delete handler_instance;
    handler_instance = nullptr;
    return true;
}

/// @brief Get a pointer reference to the instance of the event subsystem
EventHandler* EventHandler::get() {
    if (handler_instance == nullptr) {
        logger::Logger::get()->fatal("Cannot retrieve reference to uninitialized EventHandler.");
        exit(1);
        return nullptr;
    }

    return handler_instance;
}

bool EventHandler::register_event(EventCode code, void* listener, event_callback callback) {
    usize n_registered = get()->_registry[code_as_usize(code)].registered_events.size();
    for (usize i = 0; i < n_registered; i++) {
        if (
            get()->_registry[code_as_usize(code)].registered_events[i].listener == listener
        ) {
            return false;
        }
    }
    
    RegisteredEvent ev = {
        .listener = listener,
        .callback = callback
    };

    get()->_registry[code_as_usize(code)].registered_events.push_back(ev);
    return true;
}

/// @brief Unregister a listener for an event
/// @param code Event Code to unregister the listener from
/// @param listener The listener we want to unregister
/// @return true if successful. False otherwise (typically that listener is not registered for that event)
bool EventHandler::unregister_event(EventCode code, void* listener) {
    if (get()->_registry[code_as_usize(code)].registered_events.size() == 0) {
        return false;
    }

    usize n_registered = get()->_registry[code_as_usize(code)].registered_events.size();
    for (usize i = 0; i < n_registered; i++) {
        const RegisteredEvent& ev = get()->_registry[code_as_usize(code)].registered_events[i];
        if (ev.listener == listener) {
            get()->_registry[code_as_usize(code)].registered_events
                .erase(get()->_registry[code_as_usize(code)].registered_events.begin() + i);
            return true;
        }
    }

    logger::Logger::get()->warn("Attempted to unregister listener for event that it is not registered for");
    return false;
}

/// @brief Fire an event for the desired code
/// @param code The code for the event we are firing
/// @param sender The sender of the event being fired
/// @param data The data we are sending along with the event
/// @return true if the event is handled. false otherwise
bool EventHandler::fire_event(EventCode code, void* sender, EventData data) {
    usize cv = code_as_usize(code);

    if (get()->_registry[cv].registered_events.size() == 0) {
        logger::Logger::get()->warn("Attempting to fire event with no listeners");
        return false;
    }

    usize n_registered = get()->_registry[cv].registered_events.size();
    for (usize i = 0; i < n_registered; i++) {
        const RegisteredEvent& ev = get()->_registry[code_as_usize(code)].registered_events[i];
        // auto func = ev.callback;
        if (ev.callback(code, sender, ev.listener, data)) {
            return true;
        }
    }

    logger::Logger::get()->warn("Fired event left unhandled.");
    return false;
}

} // core namespace
} // gravity namespace