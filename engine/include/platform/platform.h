#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "renderer/renderer.h"
// #include "core/events.h"

#include <string>
#include <iostream>
#include <memory>
#include <optional>
#include <functional>
#include <unordered_map>

#if defined(Q_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace gravity {
/// @brief defined in `renderer/renderer.h` and implimeneted in various files for each renderer backend
namespace renderer {
    class Renderer;
}

namespace platform {

/// @brief Information relevant for the creation of a window
struct WindowPacket {
    u32 width;
    u32 height;
    std::string name;
#if defined(Q_PLATFORM_WINDOWS)
    HWND hwindow;
    HINSTANCE hinstance;

#elif defined (Q_PLATFORM_LINUX)
#endif
};

/// @brief Types of errors that can occur when handling a Window object
enum class WindowError {
    REGISTRATIONFAILED,
    CREATIONFAILED,

    TOTAL,
};


/// @brief Information for the window handle that is platform-dependent
struct WindowHandle {
#ifdef Q_PLATFORM_LINUX
    Display* m_display;
    XWindow m_window;
    int m_screen;

    void _handle_x11_event(XEvent& ev);
    Keys _translate_key(u32 code);

#elif Q_PLATFORM_WINDOWS
    HWND hwindow;
    HINSTANCE hinstance;
    // static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    
#endif // Platform Detection macros
};

/// @brief Window is the GUI window that is opened and contains all the events
class Window {
public:
    static Result<Window*, WindowError> create(u32 width, u32 height, std::string title);
    
    Window(const Window&) = delete; // do not allow the window to be copied
    Window(Window&& other); // Platform-dependent
    ~Window();

    bool operator=(const Window& other) const {
        return m_title == other.m_title;
    }

    /// @brief allow the window to be resized
    void allow_resize() { m_can_resize = true; }

    /// @brief open and show the window
    void show();

    /// @brief Set the title of the window
    void set_title(const std::string& title);

    /// @brief Shutdown behavior for the Window
    void shutdown();

    /// @brief Whether or not the window should be closed
    bool should_close();

    /// @brief Pump the window event messages to be handled
    bool pump_messages();

    const WindowHandle& get_handle() const { return m_handle; }

    /// @brief Get the title of the window
    /// @return Const reference to the title string
    const std::string& title() const { return m_title; }

    /// @brief Shutdown behavior when closing the window
    void close() {
        m_should_close = true; 
        m_is_initialized = false;
        m_renderer->shutdown();
    }

private:
    Window(const WindowPacket& info);
    
    u32 m_width, m_height;        // the dimensions of the window
    std::string m_title;          // the  title of the window to be displayed at the top
    bool m_can_resize;            // whether we are allowed to resize the window
    bool m_is_initialized;        // Whether the window is initialized properly yet
    bool m_should_close;          // whether the window should close
    WindowHandle m_handle;          // platform-specific information for the window handle
    renderer::Renderer* m_renderer; // renderer to draw to the window

    // Platform-Specific methods and members
#ifdef Q_PLATFORM_LINUX
    // Display* m_display;
    // XWindow m_window;
    // int m_screen;

    void _handle_x11_event(XEvent& ev);
    Keys _translate_key(u32 code);

#elif Q_PLATFORM_WINDOWS
    // HWND m_window;
    // HINSTANCE m_hinstance;
    // const Window& _get_window_from_hwnd(HWND hwnd);
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif // Platform Detection macros
};

/// @brief The list of colors that are able to be printed to the console
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

/// @brief Subsystem for handling platform-dependent tasks
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

    const Window* get_primary_window() const { 
        auto w = _windows.find(_primary_window_name);
        if (w == _windows.end()) {
            std::printf("Platform lost primary window\n");
            exit(1);
        }
        return w->second;
    }

    #if defined(Q_PLATFORM_WINDOWS)
    const Window& get_window_from_hwnd(HWND hwnd);
    #endif
private:
    Platform() {}
    ~Platform() = default;

    static Platform* instance;

    
    // MEMBERS //
    std::string _primary_window_name { "" };                           // name of the platform's primary window
    std::unordered_map<std::string, Window*> _windows; // table of all created windows keyed on their names
    double clock_frequency;
    
    #if defined(Q_PLATFORM_WINDOWS)
    LARGE_INTEGER start_time;
    #endif // Q_PLATFORM_WINDOWS
};

} // platform namespace
} // gravity namespace

namespace std {
    template<>
    struct hash<gravity::platform::Window> {
        size_t operator()(const gravity::platform::Window& wnd) const {
            return hash<std::string>()(wnd.title());
        }
    };
} // std namespace