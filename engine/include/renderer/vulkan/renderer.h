#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "renderer/renderer.h"

namespace gravity {
namespace renderer {
namespace vulkan {

class VK_Renderer : public Renderer {
public:
    bool startup(const config& conf) override;
    void shutdown() override;

    void begin_frame() override;
    void end_frame() override;
    void present() override;
private:

};


} // vulkan namespace
} // renderer namespace
} // gravity namespace