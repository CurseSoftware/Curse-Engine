#include "core/defines.h"
#include "core/platform.h"
#include <string>
#include <string.h>

#ifdef Q_PLATFORM_LINUX

namespace qlogger {

namespace platform {
constexpr const char* RED = "\x1b[31m";
constexpr const char* BLUE = "\x1b[34m";
constexpr const char* MAGENTA = "\x1b[35m";
constexpr const char* CYAN = "\x1b[36m";
constexpr const char* WHITE = "\x1b[37m";
constexpr const char* BLACK = "\x1b[30m";
constexpr const char* YELLOW = "\x1b[33m";
constexpr const char* GREEN = "\x1b[32m";
constexpr const char* DEFAULT = "\x1b[0m";
constexpr const char* NO_COLOR = "";
constexpr const char ENDL = '\n';

const char* color_to_cstr(color c) {
    switch (c) {
        case color::RED:
            return RED;
        case color::BLUE:
            return BLUE;
        case color::MAGENTA:
            return MAGENTA;
        case color::CYAN:
            return CYAN;
        case color::WHITE:
            return WHITE;
        case color::BLACK:
            return BLACK;
        case color::YELLOW:
            return YELLOW;
        case color::GREEN:
            return GREEN;
        
        case color::NONE:
        default:
            return NO_COLOR;
    }
}


/* @description Write a message to the console 
 * @param msg The message to write
 */
void console_write(color msg_color, const std::string& msg) {
    fprintf(
        stdout,
        "%s%s%s",
        color_to_cstr(msg_color),
        msg.c_str(),
        DEFAULT
    );
}

/* @description Write a error message to the console 
 * @param err The message to write
 */
void console_error(color msg_color, const std::string& msg) {
    fprintf(
        stdout,
        "%s%s%s",
        color_to_cstr(msg_color),
        msg.c_str(),
        DEFAULT
    );
}

} // platform namespace
  
} // gravity namespace

#endif // Q_PLATFORM_LINUX