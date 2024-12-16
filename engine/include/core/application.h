#pragma once
#include "result.h"
#include "defines.h"
#include "logger.h"
#include "types.h"
#include "input.h"
#include "window.h"
#include "platform/platform.h"

#include <map>

namespace gravity {
namespace core {

enum class ApplicationError {
    WINDOW_REGISTER = 0,
};

class Application {
public:
    static void startup() noexcept;
    static void shutdown() noexcept;

    /// @brief Create and register a window to the application
    /// @param name The name of the window
    /// @param width The desired width of the window
    /// @param height The desired height of the window
    /// @return A result of the window
    Result<Window, ApplicationError> register_window(
        std::string name,
        u32 width,
        u32 height
    ) noexcept;

private:
    Application() noexcept;

    static Application* instance;

    std::map<std::string, Window> windows;

};


} // core namespace
} // gravity namespace