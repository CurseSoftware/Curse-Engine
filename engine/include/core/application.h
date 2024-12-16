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
    static Application* startup(const std::string& name, u32 width, u32 height) noexcept;
    static void shutdown() noexcept;
    static Application* get() {
        if (!Application::instance) {
            logger::Logger::get()->error("Unable to retrieve uninitialized application");
            exit(1);
        }

        return Application::instance;
    }

    void run();

private:
    Application() noexcept;

    static Application* instance;

    std::map<std::string, Window> windows;

};


} // core namespace
} // gravity namespace