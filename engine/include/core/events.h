#pragma once
#include "defines.h"
#include "types.h"
#include "platform/platform.h"
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>

// using namespace gravity::core::types;

namespace gravity {
namespace platform {
    class Window;
}

namespace core {

enum class EventType {
    APPLICATION_QUIT,
    WINDOW_CLOSED,
    WINDOW_RESIZED,
    WINDOW_FOCUSED,
    KEY_PRESSED,
    KEY_RELEASED,
    MOUSE_MOVE,
    MOUSE_BUTTON_PRESSED,
    MOUSE_BUTTON_RELEASED,
    MOUSE_WHEEL,
};

enum class EventPriority {
    CRITICAL = 0,   // Must be handled immediately (e.g., crash events)
    HIGH = 1,       // Important system events (physics, core state changes)
    NORMAL = 2,     // Standard gameplay events
    LOW = 3,        // Visual/audio effects, non-critical updates
    DEBUG = 4       // Logging, development events
};


struct EventPropagation {
    bool handled = false;
    bool propogate = true;
    bool async = false; // TODO: async
};

class Event {
public:
    explicit Event(EventType type)
        : _type(type)
        , _priority(EventPriority::NORMAL)
        {}
    virtual ~Event() = default;

    virtual const platform::Window& source_window() const = 0;

    EventType type() const { return _type; }
    EventPropagation propogation() const { return _propagation; }
    EventPriority priority() const { return _priority; };

protected:
    EventType _type;
    EventPropagation _propagation;
    EventPriority _priority;
};

struct EventContext {
};

using EventCallback = std::function<bool(Event&, EventContext&)>;

struct CallbackData {
    EventCallback callback;
    std::string handler_name;
    EventPriority priority;
    u32 order_with_priority;

    bool operator=(const CallbackData& other) const {
        if (priority != other.priority)
            return priority < other.priority;

        return order_with_priority < other.order_with_priority;
    }

    bool operator<(const CallbackData& other) const {
        if (priority != other.priority)
            return priority < other.priority;

        return order_with_priority < other.order_with_priority;
    }
};

class EventHandler {
public:
    EventHandler() 
        : _callbacks()
        , _events()
    {}

    void register_callback(
        const platform::Window* window,
        EventType type,
        EventCallback callback,
        const std::string& handler_name,
        EventPriority priority = EventPriority::NORMAL
    ) {
        // std::printf("HERE");
        if (_callbacks.find(window) == _callbacks.end()) {
            std::cout << "Window not in event system yet, registering now.\n";
            _callbacks[window] = std::unordered_map<EventType, std::vector<CallbackData>>();
        }
        CallbackData data {
            .callback = callback,
            .handler_name = handler_name,
            .priority = priority,
            .order_with_priority = static_cast<u32>(_callbacks[window][type].size()),
        };

        _callbacks[window][type].push_back(data);
        std::sort(_callbacks[window][type].begin(), _callbacks[window][type].begin());
    }

    void post_event(std::unique_ptr<Event> event, bool immediate=false) {
        _events.push(std::move(event));
    }

    void poll_events() {
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

    static bool startup() {
        if (instance != nullptr) {
            // logger::Logger::get()->error("Attempting startup for Event system after initialization.");
            return false;
        }

        instance = new EventHandler();
    }

    static EventHandler* get() {
        if (instance == nullptr) {
            // logger::Logger::get()->fatal("Attempting to get reference to uninitialized EventHandler system");
            exit(1);
        }

        return instance;
    }

    static void shutdown() {
        if (instance == nullptr) {
            // logger::Logger::get()->fatal("Attempting to shutdown uninitialized EventHandler system");
            exit(1);
        }

        delete instance;
        instance = nullptr;
    }
private:
    std::unordered_map<
        const platform::Window*,
        std::unordered_map< EventType, std::vector<CallbackData> >
    > _callbacks;
    std::queue<std::unique_ptr<Event>> _events;
    bool is_initialized { false };

    void _process_event(Event& ev) {
        auto& source_window = ev.source_window();
        _process_window_event(ev, &source_window);

        if (ev.propogation().propogate) {
            _process_window_event(ev, nullptr);
        }
    }

    void _process_window_event(Event& ev, const platform::Window* window) {
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

    static EventHandler* instance;
};


// enum class EventCode : usize {
//     APPLICATION_QUIT = 0x01,

//     // Usage: uint16_t key_code = data.u16[0];
//     KEY_PRESSED = 0x02,

//     // Usage: uint16_t key_code = data.u16[0];
//     KEY_RELEASED = 0x03,

//     // Usage: uint16_t button = data.u16[0];
//     BUTTON_PRESSED = 0x04,

//     // Usage: uint16_t button = data.u16[0];
//     BUTTON_RELEASED = 0x05,

//     // Usage:
//     //        uint32_t mouse_x = data.u32[0];
//     //        uint32_t mouse_y = data.u32[1];
//     MOUSE_MOVE = 0x06,

//     // Usage: uint8_t z_delta = data.u8[0];
//     MOUSE_WHEEL = 0x07,

//     // Usage: 
//     //        uint16_t width = data.u16[0];
//     //        uint16_t height = data.u16[1];
//     RESIZED = 0x08,
    
//     MAX_EVENT_CODE = 0xFF
// };

// // Substitution macro for ease of getting number of event codes
// #define EVENT_CODE_AMOUNT static_cast<usize>(EventCode::MAX_EVENT_CODE)
// // constexpr usize EVENT_CODE_AMOUNT = static_cast<usize>(EventCode::MAX_EVENT_CODE);

// // Get the event code as a usize
// inline usize code_as_usize(EventCode code) {
//     return static_cast<usize>(code);
// }

// struct EventData {
//     union {
//         int64_t  i64[2];
//         uint64_t u64[2];
//         double   f64[2];

//         int32_t  i32[4];
//         uint32_t u32[4];
//         float    f32[4];

//         int16_t  i16[8];
//         uint16_t u16[8];
        
//         int8_t  i8[16];
//         uint8_t u8[16];
//     };
// };

// // Callback function object for dispatching
// using event_callback = std::function<bool (EventCode code, void* sender, void* listener, EventData data)>;

// struct RegisteredEvent {
//     void* listener;
//     event_callback callback;
// };

// // Holds information for each specific event code
// struct EventCodeEntry {
//     // The events registered for this particular code
//     std::vector<RegisteredEvent> registered_events;
// };

// class EventHandler {
// public:
//     static bool startup();
//     static bool shutdown();
//     static EventHandler* handler_instance;
//     static EventHandler* get();

//     bool register_event(EventCode code, void* listener, event_callback callback);
//     bool unregister_event(EventCode code, void* listener);
//     bool fire_event(EventCode code, void* sender, EventData data);
    

// protected:
//     EventHandler() = default;
//     ~EventHandler() = default;
// private:

//     bool m_is_initialized = false;
//     EventCodeEntry _registry[EVENT_CODE_AMOUNT];
// };

} // core namespace
} // gravity namespace