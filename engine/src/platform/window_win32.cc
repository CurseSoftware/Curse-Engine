#include "core/window.h"
#include "core/defines.h"
#include "core/input.h"

#ifdef Q_PLATFORM_WINDOWS

namespace gravity {
namespace core {

using namespace logger;
// InputHandler* input_handler = InputHandler::get_reference();

bool window_should_close = false;

constexpr const char* WINDOW_CLASS_NAME = "BIFROST WINDOW CLASS NAME";

Window::~Window() {
	if (m_is_initialized) {
		Logger::get()->warn("Window destructor called without explicit shutdown. Shutting down now.");
		this->shutdown();
	}
}

void Window::_init() {
	// input_handler = InputHandler::get_reference();
	Logger::get()->info("Creating window.");

	m_hinstance = GetModuleHandle(0);

	// Setup and register the window class
	HICON icon = LoadIcon(m_hinstance, IDI_APPLICATION);
	WNDCLASSA wc{ 0 };
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = Window::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = icon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClassA(&wc)) {
		MessageBoxA(
			0,
			"Window registration failed",
			"Error!",
			MB_ICONEXCLAMATION | MB_OK
		);
		return;
	}
	Logger::get()->info("Window registration succeeded.");

	// Create window
	uint32_t client_x = 300;
	uint32_t client_y = 100;
	uint32_t client_width = m_width;
	uint32_t client_height = m_height;

	uint32_t window_x = client_x;
	uint32_t window_y = client_y;
	uint32_t window_width = client_width;
	uint32_t window_height = client_height;

	uint32_t window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
	uint32_t window_ex_style = WS_EX_APPWINDOW;

	window_style |= WS_MAXIMIZEBOX;
	window_style |= WS_MINIMIZEBOX;
	window_style |= WS_THICKFRAME;

	RECT border_rect = { 0,0,0,0 };
	AdjustWindowRectEx(&border_rect, window_style, FALSE, window_ex_style);

	window_x += border_rect.left;
	window_y += border_rect.top;

	// Grow by the size of the OS border
	window_width += border_rect.right - border_rect.left;
	window_height += border_rect.bottom - border_rect.top;

	m_window = CreateWindowExA(
		window_ex_style,
		WINDOW_CLASS_NAME,
		m_title.c_str(),
		window_style,
		window_x,
		window_y,
		window_width,
		window_height,
		nullptr,
		nullptr,
		m_hinstance,
		nullptr
	);

	if (m_window == nullptr) {
		Logger::get()->error("Failed to create window");
		MessageBoxA(NULL, "Window creation failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	Logger::get()->info("Window created successfully");
	Logger::get()->info("Window is initialized");

	m_is_initialized = true;
}

// Gracefully exit the window shutdown
void Window::shutdown() {
	if (!m_is_initialized) {
		Logger::get()->warn("Calling shutdown on uninitialized Window. Aborting");
		return;
	}

	Logger::get()->info("Shutting down window.");
	m_is_initialized = false;
}

// Open and display the window
void Window::show() {
	if (!m_is_initialized) {
		Logger::get()->error("Attempting to show uninitialized Window. Aborting");
		return;
	}

	bool should_activate = true;
	int32_t show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;

	Logger::get()->info("Showing window");
	m_should_close = false;
	window_should_close = false;
	ShowWindow(m_window, show_window_command_flags);
}

/*
* @brief Pump messages from the window and return whether or not we should shut the window down.
*/
bool Window::should_close() {
	pump_messages();
	return window_should_close;
}

// Handle messages from the window
bool Window::pump_messages() {
	MSG message;

	// takes messages from the queue and pumps it to the application
	while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}

	return true;
}

// Callback function to handle received messages
LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_ERASEBKGND: // notify the OS that erasing the screen will be handled by Application
		return 1;

	case WM_CLOSE:
	{
		window_should_close = true;
		return true;
	} break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE: {
		RECT r;
		GetClientRect(hWnd, &r);
		u32 width = r.right - r.left;
		u32 height = r.bottom - r.top;

		InputHandler::get()->process_window_resize(
			width,
			height
		);
	} break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP: {
		bool pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
		Keys key = static_cast<Keys>(wParam);

		// Pass the input subsystem
		InputHandler::get()->process_key(key, pressed);
	} break;

	case WM_MOUSEMOVE:
		// Fire an event for mouse movement
		break;
	case WM_MOUSEWHEEL:
		// Fire an event for mouse movement
		break;

	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDOWN: {
		// TODO: fire events for mouse buttons
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

} // core namespace
} // bifrost namespace

#endif // Q_PLATFORM_WINDOWS