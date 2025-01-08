#include "core/events/events.h"
#include "core/defines.h"
#include "core/logger.h"


namespace gravity {
namespace core {

EventHandler* EventHandler::instance = nullptr;

/// @brief Register a function to execute when an event for a window was triggered
/// @param window The window that we want to register this function to
/// @param type The type of event
/// @param callback The function to execute and register
/// @param handler_name The name of the handler
/// @param priority The priority we want this function to hold
void EventHandler::register_callback(
    const platform::Window* window,
    EventType type,
    EventCallback callback,
    const std::string& handler_name,
    EventPriority priority
) {
    CallbackData data {
        .callback = callback,
        .handler_name = handler_name,
        .priority = priority,
        .order_with_priority = static_cast<u32>(_callbacks[window][type].size()),
    };

    _callbacks[window][type].push_back(data);
    std::sort(_callbacks[window][type].begin(), _callbacks[window][type].begin());
}

/// @brief Post an event
/// @param event The event
/// @param immediate Whether this should execute immediately
void EventHandler::post_event(std::unique_ptr<Event> event, bool immediate) {
    _events.push(std::move(event));
}

/// @brief Poll all outstanding events and handle them
void EventHandler::poll_events() {
    std::vector<std::unique_ptr<Event>> current_events;

    while (!_events.empty()) {
        current_events.push_back(std::move(_events.front()));
        _events.pop();
    }

    std::sort(current_events.begin(), current_events.end(), 
        [](const auto& a, const auto& b) {
            return a->priority() < b->priority();
        }
    );

    for (auto& ev : current_events) {
        _process_event(*ev);
    }
}

/// @brief Startup behavior for event system
/// @return True if successful. False otherwise
bool EventHandler::startup() {
    if (instance != nullptr) {
        // logger::Logger::get()->error("Attempting startup for Event system after initialization.");
        return false;
    }

    instance = new EventHandler();
}

/// @brief Get pointer to event system
/// @return const pointer reference to event handler
EventHandler* EventHandler::get() {
    if (instance == nullptr) {
        // logger::Logger::get()->fatal("Attempting to get reference to uninitialized EventHandler system");
        exit(1);
    }

    return instance;
}

/// @brief Shutdown behavior for event system
void EventHandler::shutdown() {
    if (instance == nullptr) {
        // logger::Logger::get()->fatal("Attempting to shutdown uninitialized EventHandler system");
        exit(1);
    }

    delete instance;
    instance = nullptr;
}

/// @brief Process an individual event
/// @param ev Event to process
void EventHandler::_process_event(Event& ev) {
    // auto& source_window = ev->source_window();
    auto& source_window = ev.source_window();
    _process_window_event(ev, &source_window);

    if (ev.propogation().propogate) {
        _process_window_event(ev, nullptr);
    }
}

/// @brief Process an event for a given window
/// @param ev Event to process
/// @param window Window to process
void EventHandler::_process_window_event(Event& ev, const platform::Window* window) {
    auto wnd_it = _callbacks.find(window);
    if (wnd_it == _callbacks.end()) {
        return;
    }

    auto type_it = wnd_it->second.find(ev.type());
    if (type_it == wnd_it->second.end()) {
        return;
    }

    for (auto& callback_data : type_it->second) {
        EventContext context;
        callback_data.callback(ev, context);
    }
}

} // core namespace
} // gravity namespace