#pragma once
#include "types.h"
#include "keys.h"
#include "mouse_buttons.h"
#include "events/events.h"
#include <tuple>
#include "logger.h"


namespace gravity {

namespace platform {
    class Window;
}

namespace core {

// State of the keys for the keyboard
struct KeyboardState {
    bool keys[Keys::KEYS_MAX_KEY] = { false };
};

// State of the mouse
struct MouseState {
    f32 x = 0.f,
        y = 0.f,
        x_prev = 0.f,
        y_prev = 0.f;
    bool buttons[MouseButtons::MAX_BUTTONS] = { false };
    f32 x_scroll = 0,
        y_scroll = 0;
};

struct WindowInputState {
    KeyboardState keyboard_curr_state;
    KeyboardState keyboard_prev_state;
    MouseState mouse_curr_state;
    MouseState mouse_prev_state;
    
    bool in_focus { false };

    platform::Window* window { nullptr };
};

// The state for the input handler
struct InputState {
    bool is_initialized;
    u32 mouse_x, 
        mouse_y;
    
    KeyboardState keyboard_curr_state;
    KeyboardState keyboard_prev_state;
    MouseState mouse_curr_state;
    MouseState mouse_prev_state;
};

// TODO: Input Handler
class InputHandler {
public:
    static void startup();
    static void shutdown();
    static InputHandler* get();
    ~InputHandler();

    void update(f64 delta_time);
    void process_key(Keys key, bool pressed);
    void process_buttons(MouseButtons button, bool pressed, platform::Window* wnd);
    void process_mouse_move(i32 x, i32 y);
    void process_mouse_wheel(i32 z_delta);
    std::tuple<f32, f32> get_mouse_position();
    void process_window_resize(u32 width, u32 height);
    void register_window(platform::Window* wnd);
    void set_focused_window(platform::Window* wnd);
private:
    bool _is_button_down(MouseButtons button);
    bool _is_button_up(MouseButtons button);
    bool _was_button_down(MouseButtons button);
    bool _was_button_up(MouseButtons button);
    
    bool _is_key_down(Keys key);
    bool _is_key_up(Keys key);
    bool _was_key_down(Keys key);
    bool _was_key_up(Keys key);

    InputState m_state;
    static InputHandler* handler_instance;
protected:
    InputHandler();

    std::unordered_map<platform::Window*, WindowInputState> _window_states;
    /// @brief Window that is currently in focus
    platform::Window* _focused_window { nullptr };

    /// @brief Window that is being hovered over 
    platform::Window* _hovered_window { nullptr };
};

class KeyPressedEvent : public Event {
public:
    KeyPressedEvent(const platform::Window& wnd, Keys k)
        : Event(EventType::KEY_PRESSED)
        , _window(wnd)
        , _key(k)
    {}

    const platform::Window& source_window() const override { return _window; }
    Keys key() const { return _key; }
private:
    const platform::Window& _window;
    Keys _key;
};

class KeyReleasedEvent : public Event {
public:
    KeyReleasedEvent(const platform::Window& wnd, Keys k)
        : Event(EventType::KEY_RELEASED)
        , _window(wnd)
        , _key(k)
    {}

    const platform::Window& source_window() const override { return _window; }
    Keys key() const { return _key; }
private:
    const platform::Window& _window;
    Keys _key;
};

class WindowFocusGainedEvent : public Event {
public:
    WindowFocusGainedEvent(const platform::Window& wnd)
        : Event(EventType::WINDOW_FOCUSED)
        , _window(wnd)
    {}

    const platform::Window& source_window() const override { return _window; }
private:
    const platform::Window& _window;
};

class WindowFocusLostEvent : public Event {
public:
    WindowFocusLostEvent(const platform::Window& wnd)
        : Event(EventType::WINDOW_UNFOCUSED)
        , _window(wnd)
    {}

    const platform::Window& source_window() const override { return _window; }
private:
    const platform::Window& _window;
};

} // core namespace
} // gravity namespace