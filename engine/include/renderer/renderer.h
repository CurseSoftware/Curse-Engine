#pragma once
#include "core/defines.h"
#include "core/types.h"

namespace gravity {

namespace platform {
    struct WindowHandle;
}
namespace renderer {

/// @brief Configuration structure for a Renderer
struct config {
    u32 width;
    u32 height;
    bool vsync;
    bool enable_debug_layer;
    platform::WindowHandle& window_handle;
};

/// @brief Interface for renderer backends to impliment
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool startup(const config& conf) = 0;
    virtual void shutdown() = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;
    virtual void present() = 0;


private:
};

constexpr u32 NUM_FRAMES = 2;

} // renderer namespace
} // gravity namespace