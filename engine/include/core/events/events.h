#pragma once
#include "core/defines.h"
#include "core/types.h"
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

/// @brief The different types of events that can occur
enum class EventType {
    APPLICATION_QUIT,
    WINDOW_CLOSED,
    WINDOW_RESIZED,
    WINDOW_FOCUSED,
    WINDOW_UNFOCUSED,
    KEY_PRESSED,
    KEY_RELEASED,
    MOUSE_MOVE,
    MOUSE_BUTTON_PRESSED,
    MOUSE_BUTTON_RELEASED,
    MOUSE_WHEEL,
};

/// @brief The priorities each event can have
enum class EventPriority {
    CRITICAL = 0,   // Must be handled immediately (e.g., crash events)
    HIGH = 1,       // Important system events (physics, core state changes)
    NORMAL = 2,     // Standard gameplay events
    LOW = 3,        // Visual/audio effects, non-critical updates
    DEBUG = 4       // Logging, development events
};

/// @brief Information relating to propogation of events
struct EventPropagation {
    bool handled = false;
    bool propogate = true;
    bool async = false; // TODO: async
};

/// @brief Interface and base class for events
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

// using EventCallback = std::function<bool(Event*, EventContext&)>;
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
    ); 
    void post_event(std::unique_ptr<Event> event, bool immediate=false);
    void poll_events();
    
    static bool startup();
    static EventHandler* get();
    static void shutdown();

private:
    std::unordered_map<
        const platform::Window*,
        std::unordered_map< EventType, std::vector<CallbackData> >
    > _callbacks;
    std::queue<std::unique_ptr<Event>> _events;
    bool is_initialized { false };

    void _process_event(Event& ev);
    void _process_window_event(Event& ev, const platform::Window* window); 

    static EventHandler* instance;
};

} // core namespace
} // gravity namespace