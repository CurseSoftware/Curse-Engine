#pragma once
#include "defines.h"
#include "types.h"

namespace gravity {

namespace platform {
    class Window;
}

namespace core {

enum class EventType {
    WINDOW_CLOSED,
    WINDOW_RESIZED,
    WINDOW_FOCUSED,
    KEY_PRESSED,
    KEY_RELEASED,
    MOUSE_MOVE,
    MOUSE_BUTTON_PRESSED,
    MOUSE_BUTTON_PRESSED,
    MOUSE_WHEEL,
};

class Event {
public:
    explicit Event(EventType type)
        : _type(type)
        {}
    virtual ~Event() = default;

    EventType type() const { return _type; }
    virtual const platform::Window& source_window() const = 0;

protected:
    EventType _type;
};

} // core namespace
} // gravity namespace