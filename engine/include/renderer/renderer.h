#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "platform/platform.h"

namespace gravity {
namespace renderer {

/// @brief Configuration structure for a Renderer
struct config {
    u32 width;
    u32 height;
    bool vsync;
    bool enable_debug_layer;
    platform::Window* window_handle;
};

/// @brief Interface for renderer backends to impliment
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual bool initialize(const config& conf) = 0;
    virtual void shutdown() = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;
    virtual void present() = 0;


private:
};

} // renderer namespace
} // gravity namespace