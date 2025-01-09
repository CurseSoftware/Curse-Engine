#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "core/logger.h"
#include "renderer/renderer.h"
#include "fx.h"

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
    void _enable_debug_layer();
    ComPtr<IDXGIAdapter4> _get_adapter(bool use_warp);
    ComPtr<ID3D12Device2> _create_device(ComPtr<IDXGIAdapter4> adapter);

    bool _is_initialized { false };
    bool _vsync { true };
    bool _tearing_supported { false };
    bool fullscreen { false };

    ComPtr<ID3D12Device2> _device;
    ComPtr<ID3D12CommandQueue> _command_queue;
    ComPtr<IDXGISwapChain4> _swapchain;
    ComPtr<ID3D12Resource> _backbuffers[NUM_FRAMES];
    ComPtr<ID3D12GraphicsCommandList> _command_list;
    ComPtr<ID3D12CommandAllocator> _command_allocators;
    ComPtr<ID3D12DescriptorHeap> _descriptor_heap;
    ComPtr<ID3D12Fence> _fence;

    ComPtr<ID3D12Debug> _debug_interface;

    u64 _fence_value = 0;
    u64 _frame_fence_values[NUM_FRAMES] = {};
    HANDLE _fence_event;

    UINT _RTVDescriptorSize;
    UINT _CurrentBackbufferIndex;
};

inline void DX_ASSERT(HRESULT hr) {
    if (FAILED(hr)) {
        core::logger::Logger::get()->fatal("DirectX Function failed.");
        std::exit(1);
    }
}


} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32