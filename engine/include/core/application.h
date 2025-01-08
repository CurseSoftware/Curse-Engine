#pragma once
#include "result.h"
#include "defines.h"
#include "logger.h"
#include "types.h"
#include "input.h"
#include "platform/platform.h"
#include "events/events.h"
#include "renderer/renderer.h"

#include <map>

namespace gravity {
namespace core {

enum class ApplicationError {
    WINDOW_REGISTER = 0,
};

class Application {
public:
    static Application* startup(const std::string& name, u32 width, u32 height) noexcept;
    static void shutdown() noexcept;
    static Application* get() {
        if (!Application::instance) {
            logger::Logger::get()->error("Unable to retrieve uninitialized application");
            exit(1);
        }

        return Application::instance;
    }

    // static bool on_event(EventCode code, void* sender, void* listener, EventData data);
    // static bool on_key(EventCode code, void* sender, void* listener, EventData data);
    // static bool on_resize(EventCode code, void* sender, void* listener, EventData data);
    // static bool on_mouse_move(EventCode code, void* sender, void* listener, EventData data);

    void run();

private:
    Application() noexcept;
    static Application* instance;

    struct {
        std::string name { "" };
        bool should_quit { false };
        bool is_running { false };
        bool is_suspended { false };
    } state;
};

class ApplicationQuitEvent : public Event {
public:
    ApplicationQuitEvent(const platform::Window& wnd)
        : Event(EventType::APPLICATION_QUIT)
        , _window(wnd)
        {}
    
    const platform::Window& source_window() const override { return _window; }
private:
    const platform::Window& _window;
};

} // core namespace
} // gravity namespace