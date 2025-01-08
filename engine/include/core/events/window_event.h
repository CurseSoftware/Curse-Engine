#pragma once
#include "events.h"
#include "platform/platform.h"

namespace gravity {
namespace core {

class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(const platform::Window& wnd, u32 width, u32 height)
        : Event(EventType::WINDOW_RESIZED)
        , _window(wnd)
        , _width(width)
        , _height(height)
    {}

    const platform::Window& source_window() const override { return _window; }
private:
    const platform::Window& _window;
    u32 _width;
    u32 _height;
};

} // core namespace
} // gravity namespace