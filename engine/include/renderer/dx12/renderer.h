#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "renderer/renderer.h"

#if defined(Q_PLATFORM_WINDOWS)

namespace gravity {
namespace renderer {
namespace dx12 {

class DX12_Renderer : public Renderer {
public:
    bool startup(const config& conf) override;
    void shutdown() override;

    void begin_frame() override;
    void end_frame() override;
    void present() override;
private:
    platform::Window* _wnd;
};


} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32