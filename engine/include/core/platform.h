#pragma once
#include "defines.h"

#include <string>

#if defined(Q_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace gravity {
namespace core {

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
            exit(1);
        } 
        return Platform::instance;
    };

    Platform(const Platform&) = delete;
    Platform& operator=(const Platform&) = delete;
    
    void console_write(color msg_color, const std::string& msg);

    void console_error(color msg_color, const std::string& err);

    double get_absolute_time();

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
} // core namespace
} // gravity namespace