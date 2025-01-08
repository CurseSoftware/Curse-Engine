// #include "core/window.h"
#include "core/defines.h"
#include "core/application.h"
#include "core/input.h"
#include "core/events/events.h"
#include "core/events/window_event.h"
#include "renderer/renderer.h"
#include "renderer/dx12/renderer.h"

#ifdef Q_PLATFORM_WINDOWS

namespace gravity {
namespace platform {

using namespace core::logger;
// InputHandler* input_handler = InputHandler::get_reference();

bool window_should_close = false;

constexpr const char* WINDOW_CLASS_NAME = "BIFROST WINDOW CLASS NAME";

/// @brief Window constructor
/// @param packet Packet containing information relevant to the window
Window::Window(const WindowPacket& packet)
	// : m_hinstance(packet.hinstance)
	// , m_window(packet.hwindow)
	: m_handle(WindowHandle{
		.hwindow = packet.hwindow,
		.hinstance = packet.hinstance,
	})
	, m_width(packet.width)
	, m_height(packet.height)
	, m_title(packet.name)
	, m_can_resize(false)
	, m_is_initialized(true)
	, m_renderer(new renderer::dx12::DX12_Renderer())
{
	m_renderer->startup(
		renderer::config {
			.width = m_width,
			.height = m_height,
			.vsync = true,
			.enable_debug_layer = true,
			.window_handle = m_handle
		}
	);
}

/// @brief Destroy the window
Window::~Window() {
	if (m_is_initialized) {
		Logger::get()->warn("Window destructor called without explicit shutdown. Shutting down now.");
		this->shutdown();
	}
}

/// @brief Move constructor
/// @param other 
Window::Window(Window&& other) {
	std::swap(m_title, other.m_title);
	std::swap(m_width, other.m_width);
	std::swap(m_height, other.m_height);
	// std::swap(m_window, other.m_window);
	// std::swap(m_hinstance, other.m_hinstance);
	std::swap(m_handle, other.m_handle);
	std::swap(m_can_resize, other.m_can_resize);
	std::swap(m_is_initialized, other.m_is_initialized);
	std::swap(m_should_close, other.m_should_close);
}



/// @brief Attempt to create a window
/// @param width Width in pixels of the window
/// @param height Height in pixels of the window
/// @param title Name of the window
/// @return Ok(Window) if successful. Err(err) otherwise.
Result<Window*, WindowError> Window::create(
	u32 width,
	u32 height,
	std::string title
) {
	Logger::get()->info("Creating window.");
	HINSTANCE hinstance = GetModuleHandle(0);

	// Setup and register the window class
	HICON icon = LoadIcon(hinstance, IDI_APPLICATION);
	WNDCLASSA wc{ 0 };
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = Window::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
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
		return Err(WindowError::REGISTRATIONFAILED);
	}
	Logger::get()->info("Window registration succeeded.");

	// Create window
	uint32_t client_x = 300;
	uint32_t client_y = 100;
	uint32_t client_width = width;
	uint32_t client_height = height;

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

	HWND hwindow = CreateWindowExA(
		window_ex_style,
		WINDOW_CLASS_NAME,
		title.c_str(),
		window_style,
		window_x,
		window_y,
		window_width,
		window_height,
		nullptr,
		nullptr,
		hinstance,
		nullptr
	);

	if (hwindow == nullptr) {
		Logger::get()->error("Failed to create window");
		MessageBoxA(NULL, "Window creation failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return Err(WindowError::CREATIONFAILED);
	}

	Logger::get()->info("Window created successfully");
	Logger::get()->info("Window is initialized");

	WindowPacket packet = { 0 };
	packet.hinstance = hinstance;
	packet.hwindow = hwindow;
	packet.width = width;
	packet.height = height;
	packet.name = title;

	// m_is_initialized = true;
	return Ok(new Window(packet));
}

/// @brief Gracefully exit the window shutdown
void Window::shutdown() {
	if (!m_is_initialized) {
		Logger::get()->warn("Calling shutdown on uninitialized Window. Aborting");
		return;
	}

	Logger::get()->info("Shutting down window.");
	m_is_initialized = false;
}

/// @brief Change the name of the window
/// @param title desired new title of the window
void Window::set_title(const std::string& title) {
	// TODO: change the name of the window
	m_title = title;	
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
	ShowWindow(m_handle.hwindow, show_window_command_flags);
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
			core::EventHandler::get()->post_event(
				std::make_unique<core::ApplicationQuitEvent>(
					core::ApplicationQuitEvent(
						*Platform::get()->get_window_from_hwnd(hWnd)
					)
				),
				true
			);
			// core::EventHandler::get()->fire_event(core::EventCode::APPLICATION_QUIT, nullptr, {});
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

			core::InputHandler::get()->process_window_resize(
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
			core::InputHandler::get()->process_key(key, pressed);
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

		case WM_SETFOCUS: {
			core::InputHandler::get()->set_focused_window(
				Platform::get()->get_window_from_hwnd(hWnd)
			);
			std::cout << "gained focused\n";
		} break;
		case WM_KILLFOCUS: {
			std::cout << "lost focused\n";
		} break;

		default: 
			break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

} // platform namespace
} // bifrost namespace

#endif // Q_PLATFORM_WINDOWS