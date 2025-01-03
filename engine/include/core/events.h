#pragma once
#include "core/defines.h"
#include "core/types.h"
#include <functional>
#include <vector>

// using namespace gravity::core::types;

namespace gravity {

namespace core {

enum class EventCode : usize {
    APPLICATION_QUIT = 0x01,

    // Usage: uint16_t key_code = data.u16[0];
    KEY_PRESSED = 0x02,

    // Usage: uint16_t key_code = data.u16[0];
    KEY_RELEASED = 0x03,

    // Usage: uint16_t button = data.u16[0];
    BUTTON_PRESSED = 0x04,

    // Usage: uint16_t button = data.u16[0];
    BUTTON_RELEASED = 0x05,

    // Usage:
    //        uint32_t mouse_x = data.u32[0];
    //        uint32_t mouse_y = data.u32[1];
    MOUSE_MOVE = 0x06,

    // Usage: uint8_t z_delta = data.u8[0];
    MOUSE_WHEEL = 0x07,

    // Usage: 
    //        uint16_t width = data.u16[0];
    //        uint16_t height = data.u16[1];
    RESIZED = 0x08,
    
    MAX_EVENT_CODE = 0xFF
};

// Substitution macro for ease of getting number of event codes
#define EVENT_CODE_AMOUNT static_cast<usize>(EventCode::MAX_EVENT_CODE)
// constexpr usize EVENT_CODE_AMOUNT = static_cast<usize>(EventCode::MAX_EVENT_CODE);

// Get the event code as a usize
inline usize code_as_usize(EventCode code) {
    return static_cast<usize>(code);
}

struct EventData {
    union {
        int64_t  i64[2];
        uint64_t u64[2];
        double   f64[2];

        int32_t  i32[4];
        uint32_t u32[4];
        float    f32[4];

        int16_t  i16[8];
        uint16_t u16[8];
        
        int8_t  i8[16];
        uint8_t u8[16];
    };
};

// Callback function object for dispatching
using event_callback = std::function<bool (EventCode code, void* sender, void* listener, EventData data)>;

struct RegisteredEvent {
    void* listener;
    event_callback callback;
};

// Holds information for each specific event code
struct EventCodeEntry {
    // The events registered for this particular code
    std::vector<RegisteredEvent> registered_events;
};

class EventHandler {
public:
    static bool startup();
    static bool shutdown();
    static EventHandler* handler_instance;
    static EventHandler* get();

    bool register_event(EventCode code, void* listener, event_callback callback);
    bool unregister_event(EventCode code, void* listener);
    bool fire_event(EventCode code, void* sender, EventData data);
    

protected:
    EventHandler() = default;
    ~EventHandler() = default;
private:

    bool m_is_initialized = false;
    EventCodeEntry _registry[EVENT_CODE_AMOUNT];
};

} // core namespace
} // gravity namespace