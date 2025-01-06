#include "renderer/dx12/renderer.h"

#if defined(Q_PLATFORM_WINDOWS)

#include "core/logger.h"

namespace gravity {
namespace renderer {
namespace dx12 {

using namespace core::logger;

/// @brief Initialize DirectX12 Renderer backend
/// @param conf Configuration of the renderer
/// @return True if successful. False otherwise
bool DX12_Renderer::startup(const config& conf) {
    Logger::get()->debug("DX12 Renderer initialized successfuly.");
    return true;
}

/// @brief Shutdown DX12 Renderer
void DX12_Renderer::shutdown() {
    Logger::get()->debug("DX12 Renderer shutdown.");
}

void DX12_Renderer::begin_frame() {

}

void DX12_Renderer::end_frame() {

}

void DX12_Renderer::present() {

}

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32