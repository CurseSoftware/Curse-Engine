#include "core/input.h"
#include "core/mouse_buttons.h"

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
        Logger::get()->fatal("Cannot perform InputHandler startup twice");
        exit(1);
    }
}

/// @brief Shutdown input subsystem;
void InputHandler::shutdown() {
    if (InputHandler::handler_instance) {
        delete InputHandler::handler_instance;
        InputHandler::handler_instance = nullptr;
    } else {
        Logger::get()->warn("Attempting to shutdown Input subsystem when not initialized");
    }
}

/// @brief Retrieve reference to the input subsystem if it is initialized.
/// @return reference to the InputHandler
InputHandler* InputHandler::get() {
    if (InputHandler::handler_instance) {
        return handler_instance;
    } else {
        Logger::get()->fatal("Cannot retrieve reference to uninitialiezd InputHandler");
        exit(1);
    }
}

// Constructor
InputHandler::InputHandler() 
{
    m_state.is_initialized = true;
    Logger::get()->info("Input subsystem is initialized.");
}

// Destructor
InputHandler::~InputHandler() {
    Logger::get()->info("Input handler shutting down");
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

void InputHandler::process_window_resize(u32 w, u32 h) {
}

// Return a tuple of the current mouse position.
// The first item in the tuple is the x coordinate 
// and the second is the y.
std::tuple<i32, i32> InputHandler::get_mouse_position() {
    return std::make_tuple(m_state.mouse_curr_state.x, m_state.mouse_curr_state.y);
}

void InputHandler::process_key(Keys key, bool pressed) {
    if (m_state.keyboard_curr_state.keys[key] != pressed) {
        m_state.keyboard_curr_state.keys[key] = pressed;

        if (pressed)
            Logger::get()->debug("Key pressed: %c", static_cast<char>(key));
        else
            Logger::get()->debug("Key released: %c", static_cast<char>(key));

        // TODO: fire event once events are setup
    }
}

// Process the mouse wheel input
void InputHandler::process_mouse_wheel(i32 z_delta) {
    (void)z_delta;
}

// Process a move of the mouse
void InputHandler::process_mouse_move(i32 x, i32 y) {
    if (m_state.mouse_curr_state.x != x
        || m_state.mouse_curr_state.y != y) {
        m_state.mouse_curr_state.x = x;
        m_state.mouse_curr_state.y = y;

        // TODO: fire event
    }
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