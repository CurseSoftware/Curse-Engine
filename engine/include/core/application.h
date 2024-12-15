#pragma once
#include "result.h"
#include "defines.h"
#include "logger.h"
#include "types.h"
#include "input.h"
#include "platform/platform.h"

namespace gravity {
namespace core {

enum class ApplicationError {
    WINDOW_REGISTER = 0,
};

class Application {
public:
    static void startup() noexcept;
    static void shutdown() noexcept;
    Result<int, ApplicationError> register_window(
        std::string name,
        u32 width,
        u32 height
    ) noexcept;

private:
    Application() noexcept;

    static Application* instance;
};


} // core namespace
} // gravity namespace