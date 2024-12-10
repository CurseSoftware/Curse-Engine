#pragma once
#include "result.h"

namespace gravity {
namespace core {

enum class ApplicationError {
    WINDOW_REGISTER = 0,
};

class Application {
public:
    static Application create() noexcept;
    result::Result<int, ApplicationError> register_window() noexcept;

private:
    Application() noexcept;
};

} // core namespace
} // gravity namespace