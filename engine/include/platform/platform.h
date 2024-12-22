#pragma once
#include "core/defines.h"
#include "core/window.h"

#include <string>
#include <iostream>

#if defined(Q_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace gravity {

namespace platform {

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
    static void startup();
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
    
    void console_write(color msg_color, const std::string& msg);

    void console_error(color msg_color, const std::string& err);

    double get_absolute_time();

    // Result<core::Window, core::WindowError> create_window(u32 width, u32 height, std::string title);

private:
    Platform() {}
    ~Platform() = default;

    static Platform* instance;

    // MEMBERS //
    /* Clock */
    double clock_frequency;
    #if defined(Q_PLATFORM_WINDOWS)
    LARGE_INTEGER start_time;
    #endif
};




} // platform namespace
} // gravity namespace