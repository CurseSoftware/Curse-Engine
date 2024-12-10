#include <iostream>
#include "core/application.h"

namespace gravity {

namespace core { 

Application Application::create() noexcept {
    Application a = Application();
    return a;
}

result::Result<int, ApplicationError> Application::register_window() noexcept {
    return result::Err(ApplicationError::WINDOW_REGISTER);
}

Application::Application() noexcept {
    std::cout << "Creating application..." << std::endl;
}

}
}

