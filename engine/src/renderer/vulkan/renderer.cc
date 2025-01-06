#include "renderer/vulkan/renderer.h"

namespace gravity {
namespace renderer {
namespace vulkan {

/// @brief Initialize the renderer
/// @param conf Configuration for the renderer
/// @return true if successful. False otherwise
bool VKRenderer::initialize(const config& conf) {
    return false;
}

/// @brief Shutdown the vulkan renderer
void VKRenderer::shutdown() {

}

void VKRenderer::begin_frame() {

}

void VKRenderer::end_frame() {

}

void VKRenderer::present() {

}

} // vulkan namespace
} // renderer namespace
} // gravity namespace