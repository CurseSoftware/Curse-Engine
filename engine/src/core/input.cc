#include "core/input.h"
#include "core/mouse_buttons.h"
#include "core/events.h"

namespace gravity {
namespace core {

using namespace logger;

// Singleton instance for the input handler
InputHandler* InputHandler::handler_instance = nullptr;

/// @brief Startup input subsystem
void InputHandler::startup() {
    if (!InputHandler::handler_instance) {
        InputHandler::handler_instance = new InputHandler();
    } else {
        // Logger::get()->fatal("Cannot perform InputHandler startup twice");
        exit(1);
    }
    
    // Logger::get()->debug("Startup input subsystem successful.");
}

/// @brief Shutdown input subsystem;
void InputHandler::shutdown() {
    if (InputHandler::handler_instance) {
        delete InputHandler::handler_instance;
        InputHandler::handler_instance = nullptr;
    } else {
        Logger::get()->warn("Attempting to shutdown Input subsystem when not initialized");
    }
    Logger::get()->debug("Shutdown input subsystem successful.");
}

/// @brief Retrieve reference to the input subsystem if it is initialized.
/// @return reference to the InputHandler
InputHandler* InputHandler::get() {
    if (InputHandler::handler_instance) {
        return handler_instance;
    } else {
        // Logger::get()->fatal("Cannot retrieve reference to uninitialiezd InputHandler");
        exit(1);
    }
}

/// @brief Constructor
InputHandler::InputHandler() 
{
    m_state.is_initialized = true;
}

/// @brief Destructor
InputHandler::~InputHandler() {
    m_state.is_initialized = false;
}

void InputHandler::update(f64 delta_time) {
    (void)delta_time;
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: attempting to update uninitialized handler.");
        return;
    }

    // Copy current state to prev
    m_state.keyboard_prev_state = m_state.keyboard_curr_state;
    m_state.mouse_prev_state = m_state.mouse_curr_state;
}

/// @brief Process the input of resizing the application window
/// @param w New width of the resizing
/// @param h New height of the resizing
void InputHandler::process_window_resize(u32 w, u32 h) {
    // EventData data = {};
    
    // data.u32[0] = static_cast<u32>(w);
    // data.u32[1] = static_cast<u32>(h);

    // EventHandler::get()->fire_event(
    //     EventCode::RESIZED,
    //     nullptr,
    //     data
    // );
}

// Return a tuple of the current mouse position.
// The first item in the tuple is the x coordinate 
// and the second is the y.

/// @brief Get the current mouse position
/// @return A tuple of the mouse position as <x, y>
std::tuple<i32, i32> InputHandler::get_mouse_position() {
    return std::make_tuple(m_state.mouse_curr_state.x, m_state.mouse_curr_state.y);
}

/// @brief Handle the input of a key being pressed or released
/// @param key The keycode of the key that was input
/// @param pressed True if the key is pressed. False if released.
void InputHandler::process_key(Keys key, bool pressed) {
    switch (key) {
        case Keys::KEY_LALT: 
            logger::Logger::get()->debug("LALT");
            break;
        case Keys::KEY_RALT: 
            logger::Logger::get()->debug("RALT");
            break;
        case Keys::KEY_LCONTROL: 
            logger::Logger::get()->debug("LCONTROL");
            break;
        case Keys::KEY_RCONTROL: 
            logger::Logger::get()->debug("RCONTROL");
            break;
        case Keys::KEY_LSHIFT: 
            logger::Logger::get()->debug("LSHIFT");
            break;
        case Keys::KEY_RSHIFT: 
            logger::Logger::get()->debug("RSHIFT");
            break;
        default: 
            break;
    }
    
    if (m_state.keyboard_curr_state.keys[key] != pressed) {
        m_state.keyboard_curr_state.keys[key] = pressed;

        // EventData data = {
        //     .u16 = static_cast<u16>(key)
        // };
        // EventHandler::get()->fire_event(
        //     pressed ? EventCode::KEY_PRESSED : EventCode::KEY_RELEASED,
        //     nullptr,
        //     data
        // );
    }
}

/// @brief Process the moving of the mouse wheel
/// @param z_delta How much the wheel has moved
void InputHandler::process_mouse_wheel(i32 z_delta) {
    (void)z_delta;
}

/// @brief Process the mouse moving
/// @param x New x coordinate of the mouse
/// @param y New y coordinate of the mouse
void InputHandler::process_mouse_move(i32 x, i32 y) {
    // if (m_state.mouse_curr_state.x != x
    //     || m_state.mouse_curr_state.y != y
    // ) {
    //     m_state.mouse_curr_state.x = x;
    //     m_state.mouse_curr_state.y = y;
        
    //     EventData data = {};
    //     data.u16[0] = static_cast<u16>(x);
    //     data.u16[1] = static_cast<u16>(y);

    //     // TODO: fire event
    //     EventHandler::get()->fire_event(
    //         EventCode::MOUSE_MOVE,
    //         nullptr,
    //         data
    //     );
    // }
}


/// PRIVATE ///

bool InputHandler::_is_button_down(MouseButtons button) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }

    return m_state.mouse_curr_state.buttons[button] == false;
}

bool InputHandler::_was_button_down(MouseButtons button) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }

    return m_state.mouse_prev_state.buttons[button] == true;
}


bool InputHandler::_is_button_up(MouseButtons button) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }

    return m_state.mouse_curr_state.buttons[button] == true;
}

bool InputHandler::_was_button_up(MouseButtons button) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }

    return m_state.mouse_prev_state.buttons[button] == false;
}

bool InputHandler::_is_key_down(Keys key) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }

    return m_state.keyboard_curr_state.keys[key] == true;
}

bool InputHandler::_was_key_down(Keys key) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }

    return m_state.keyboard_prev_state.keys[key] == true;
}

bool InputHandler::_is_key_up(Keys key) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }

    return m_state.keyboard_curr_state.keys[key] == true;
}

bool InputHandler::_was_key_up(Keys key) {
    if (!m_state.is_initialized) {
        Logger::get()->warn("InputHandler: trying to access uninitialized handler.");
        return false;
    }
    
    return m_state.keyboard_curr_state.keys[key] == false;
}

} // core namespace
} // gravity namespace