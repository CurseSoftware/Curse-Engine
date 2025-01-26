#include "core/logger.h"
#include "platform/platform.h"
#include "core/defines.h"
#include "core/input.h"
#include "renderer/dx12/renderer.h"

#ifdef Q_PLATFORM_WINDOWS
#include <windows.h>
#include <cstdint>
namespace gravity {

namespace platform {
// using namespace core;

Platform* Platform::instance = nullptr;
static double clock_frequency;

constexpr int COLOR_BLACK = 0;
constexpr int COLOR_BLUE = 1;
constexpr int COLOR_GREEN = 2;
constexpr int COLOR_AQUA = 3;
constexpr int COLOR_RED = 4;
constexpr int COLOR_PURPLE = 5;
constexpr int COLOR_YELLOW = 6;
constexpr int COLOR_WHITE = 7;
constexpr int COLOR_GRAY = 8;
constexpr int COLOR_LIGHT_BLUE = 9;
constexpr int COLOR_LIGHT_GREEN = 10;
constexpr int COLOR_LIGHT_AQUA = 11;
constexpr int COLOR_LIGHT_RED = 12;
constexpr int COLOR_LIGHT_PURPLE = 13;
constexpr int COLOR_LIGHT_YELLOW = 14;
constexpr int COLOR_BRIGHT_WHITE = 15;

/// @brief Convert color object to the ascii value for printing
/// @param c Color object to convert
/// @return Ascii code for printing color to console
constexpr int color_to_value(color c) {
    switch (c) {
        case color::RED:
            return COLOR_RED;
        case color::BLUE:
            return COLOR_BLUE;
        case color::MAGENTA:
            return COLOR_LIGHT_PURPLE;
        case color::CYAN:
            return COLOR_LIGHT_AQUA;
        case color::WHITE:
            return COLOR_BRIGHT_WHITE;
        case color::YELLOW:
            return COLOR_YELLOW;
        case color::GREEN:
            return COLOR_GREEN;
        
        case color::NONE:
        default:
            return 7;
    }
}

/// @brief Write a message to the console
/// @param msg_color color of the message
/// @param msg The message text to write
void Platform::console_write(color msg_color, const std::string& msg) {
    CONSOLE_SCREEN_BUFFER_INFO Info;
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console_handle, &Info);
	WORD Attributes = Info.wAttributes;

	SetConsoleTextAttribute(console_handle, color_to_value(msg_color));

	OutputDebugStringA(msg.c_str());
	uint64_t length = msg.length();
	LPDWORD number_written = 0;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), msg.c_str(), (DWORD)length, number_written, 0);
	SetConsoleTextAttribute(console_handle, Attributes);
}

/// @brief Write error to the console
/// @param msg_color Color of the message 
/// @param msg Message text
void Platform::console_error(color msg_color, const std::string& msg) {
    CONSOLE_SCREEN_BUFFER_INFO Info;
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console_handle, &Info);
	WORD Attributes = Info.wAttributes;

	SetConsoleTextAttribute(console_handle, color_to_value(msg_color));

	OutputDebugStringA(msg.c_str());
	uint64_t length = msg.length();
	LPDWORD number_written = 0;
	WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), msg.c_str(), (DWORD)length, number_written, 0);
	SetConsoleTextAttribute(console_handle, Attributes);
}

/// @brief Get the absolute time since 1969 in epoch
double Platform::get_absolute_time() {
	LARGE_INTEGER now_time;
	QueryPerformanceCounter(&now_time);
	return static_cast<double>(now_time.QuadPart) * clock_frequency;
}

/// @brief Startup behavior for the Win32 Platform
void Platform::startup(const std::string& name, u32 width, u32 height) {
    if (!Platform::instance) {
        Platform::instance = new Platform();
    } else {
        exit(1);
    }

    LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	Platform::get()->clock_frequency = 1.0f / static_cast<double>(frequency.QuadPart);
	QueryPerformanceCounter(&Platform::get()->start_time);

    Platform::instance->_primary_window_name = name;
    Platform::instance->_windows[name] = Window::create(width, height, name).unwrap();
    // Platform::instance->_windows[name] = std::make_unique<Window>(Window::create(width, height, name).unwrap());
    core::InputHandler::get()->register_window(Platform::instance->_windows[name]);
    Platform::instance->_windows[name]->show();
    std::vector<std::string> keys;
    
    core::logger::Logger::get()->debug("Startup platform <Win32> successful.");
}

/// @brief Pump messages to places that need it
void Platform::pump_messages() {
    for (const auto& window : Platform::get()->_windows) {
        window.second->pump_messages();
        window.second->draw_frame();
    }
}

/// @brief Shutdown behavior for the Win32 platform
void Platform::shutdown() {
    if (Platform::instance) {
        core::logger::Logger::get()->debug("Shutdown platform <Win32> successful.");
        Platform::instance->_windows[Platform::get()->_primary_window_name]->close();
        delete Platform::instance;
        Platform::instance = nullptr;
    }
}

/// @brief Find window from the hwnd handle
/// @param hwnd handle to find by
/// @return constant reference to the window
Window* Platform::get_window_from_hwnd(HWND hwnd) {
    for (auto it = _windows.begin(); it != _windows.end(); it++) {
        if (it->second->get_handle().hwindow == hwnd) {
            return it->second;
        }
    }

    core::logger::Logger::get()->fatal("Attempting to find platform window from hwnd that does not match any in window list.");
    exit(1);
}

} // platform namespace
} // gravity namespace
#endif // Q_PLATFORM_WINDOWS