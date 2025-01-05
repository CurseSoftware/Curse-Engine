#pragma once
#include "core/defines.h"
// #include "core/window.h"
#include "core/types.h"

#include <string>
#include <iostream>
#include <memory>
#include <optional>

#if defined(Q_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace gravity {

namespace platform {

struct WindowPacket {
    u32 width;
    u32 height;
#if defined(Q_PLATFORM_WINDOWS)
    HWND hwindow;
    HINSTANCE hinstance;

#elif defined (Q_PLATFORM_LINUX)
#endif
};

/// Types of errors that can occur when handling a Window object
enum class WindowError {
    REGISTRATIONFAILED,
    CREATIONFAILED,

    TOTAL,
};

/// Window is the GUI window that is opened and contains all the events
class Window {
public:
    static Result<Window, WindowError> create(u32 width, u32 height, std::string title);
    
    Window(const Window&) = delete; // do not allow the window to be copied
    Window(Window&& other); // Platform-dependent

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

    void close() { m_should_close = true; }

private:
    Window(const WindowPacket& info);
    
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

// The list of colors that are able to be printed to the console
enum class color : int {
    RED,
    MAGENTA,
    BLUE,
    CYAN,
    WHITE,
    BLACK,
    YELLOW,
    GREEN,
    NONE,

    NUM_COLORS
};

class Platform {
public:
    static void startup(const std::string& name, u32 width, u32 height);
    static void shutdown();

    /// @return singleton instance of the Platofrm
    static Platform* get() {
        if (!Platform::instance) {
            std::cout << "NO PLATFORM\n";
            exit(1);
        } 
        return Platform::instance;
    };

    Platform(const Platform&) = delete;
    Platform& operator=(const Platform&) = delete;

    void pump_messages();
    void console_write(color msg_color, const std::string& msg);

    void console_error(color msg_color, const std::string& err);

    double get_absolute_time();

    // Result<core::Window, core::WindowError> create_window(u32 width, u32 height, std::string title);

private:
    Platform() {}
    ~Platform() = default;

    static Platform* instance;

    std::string _primary_window_name { "" };
    std::unordered_map<std::string, std::unique_ptr<Window>> _windows;
    // std::unique_ptr<Window> wnd = nullptr;
    // std::optional<std::unique_ptr<core::Window>> wnd;

    // MEMBERS //
    /* Clock */
    double clock_frequency;
    #if defined(Q_PLATFORM_WINDOWS)
    LARGE_INTEGER start_time;
    #endif
};




} // platform namespace
} // gravity namespace