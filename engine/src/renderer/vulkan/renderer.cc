#include "renderer/vulkan/renderer.h"

namespace gravity {
namespace renderer {
namespace vulkan {

/// @brief Initialize the renderer
/// @param conf Configuration for the renderer
/// @return true if successful. False otherwise
bool VK_Renderer::startup(const config& conf) {
    return false;
}

/// @brief Shutdown the vulkan renderer
void VK_Renderer::shutdown() {

}

void VK_Renderer::begin_frame() {

}

void VK_Renderer::end_frame() {

}

void VK_Renderer::present() {

}

} // vulkan namespace
} // renderer namespace
} // gravity namespace