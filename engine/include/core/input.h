#pragma once
#include "types.h"
#include "keys.h"
#include "mouse_buttons.h"
#include <tuple>
#include "logger.h"


namespace gravity {

namespace core {

// State of the keys for the keyboard
struct KeyboardState {
    bool keys[Keys::KEYS_MAX_KEY] = { false };
};

// State of the mouse
struct MouseState {
    i32 x, y;
    bool buttons[MouseButtons::MAX_BUTTONS] = { false };
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
    void process_buttons(MouseButtons button, bool pressed);
    void process_mouse_move(i32 x, i32 y);
    void process_mouse_wheel(i32 z_delta);
    std::tuple<i32, i32> get_mouse_position();
    void process_window_resize(u32 width, u32 height);
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
};

} // core namespace

} // gravity namespace