#pragma once
#include "result.h"
#include "defines.h"
#include "logger.h"
#include "platform.h"

namespace gravity {
namespace core {

enum class ApplicationError {
    WINDOW_REGISTER = 0,
};

class Application {
public:
    static void startup() noexcept;
    static void shutdown() noexcept;
    result::Result<int, ApplicationError> register_window() noexcept;

private:
    Application() noexcept;

    static Application* instance;
};


} // core namespace
} // gravity namespace