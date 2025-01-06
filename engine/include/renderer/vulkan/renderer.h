#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "renderer/renderer.h"

namespace gravity {
namespace renderer {
namespace vulkan {

class VKRenderer : public IRenderer {
public:
    bool initialize(const config& conf) override;
    void shutdown() override;

    void begin_frame() override;
    void end_frame() override;
    void present() override;
private:
    platform::Window* _wnd;


};


} // vulkan namespace
} // 
}