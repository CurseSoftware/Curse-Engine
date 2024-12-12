#pragma once
#include "input.h"
#include "keys.h"
#include "types.h"
#include "defines.h"
#include "logger.h"

// Platform Specific includes
#ifdef Q_PLATFORM_LINUX
#include <X11/Xlib.h>
#include <X11/X.h>
#include <string>

using XWindow = Window;
#elif Q_PLATFORM_WINDOWS
#include <windows.h>
#endif // Platform Detection macros


namespace gravity {
namespace core {

/// Window is the GUI window that is opened and contains all the events
class QAPI Window {
public:
    Window(u32 width, u32 height, std::string title)
        : m_width(width)
        , m_height(height)
        , m_title(title)
        , m_can_resize(false)
        , m_is_initialized(false)
        /* , m_input(InputHandler()) */
        , m_should_close(false)
    {
        _init();
    }
    Window(const Window&) = delete; // do not allow the window to be copied
    Window(Window&& other);

    ~Window();

    // allow the window to be resized
    void allow_resize() { m_can_resize = true; }

    // open and show the window
    void show();

    // Set the title of the window
    void set_title(const std::string& title);

    // Shutdown behavior for the Window
    void shutdown();

    // Whether or not the window should be closed
    bool should_close();

    // Pump the window event messages to be handled
    bool pump_messages();

private:
    
    u32 m_width, m_height; // the dimensions of the window
    std::string m_title;   // the  title of the window to be displayed at the top
    bool m_can_resize;     // whether we are allowed to resize the window
    bool m_is_initialized; // Whether the window is initialized properly yet
    bool m_should_close;   // whether the window should close

    // Platform-Specific methods and members
#ifdef Q_PLATFORM_LINUX
    Display* m_display;
    XWindow m_window;
    int m_screen;

    void _handle_x11_event(XEvent& ev);
    Keys _translate_key(u32 code);

#elif Q_PLATFORM_WINDOWS
    HWND m_window;
    HINSTANCE m_hinstance;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    
#endif // Platform Detection macros

    void _init();
};

} // core namespace
} // gravity namespace