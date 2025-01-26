#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "core/logger.h"
#include "renderer/renderer.h"
#include "fx.h"
#include "dx.h"
#include <chrono>

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

    void resize(u32 width, u32 height);
private:
    UINT _use_warp { false };

    CD3DX12_VIEWPORT _viewport;
    CD3DX12_RECT _scissor_rect;
    ComPtr<IDXGISwapChain3> _swapchain;
    ComPtr<ID3D12Device> _device;
    ComPtr<ID3D12Resource> _render_targets[NUM_FRAMES];
    ComPtr<ID3D12CommandAllocator> _command_allocator;
    ComPtr<ID3D12CommandQueue> _command_queue;
    ComPtr<ID3D12RootSignature> _root_signature;
    ComPtr<ID3D12DescriptorHeap> _rtv_heap;
    ComPtr<ID3D12PipelineState> _pipeline_state;
    ComPtr<ID3D12GraphicsCommandList> _command_list;
    UINT _rtv_descriptor_size;

    HANDLE _fence_event;
    ComPtr<ID3D12Fence> _fence;
    UINT64 _fence_value;

    struct {
        u32 current_width;
        u32 current_height;
        UINT frame_index;
        HWND hwnd;
    } _state;

    /* METHODS */
    void _load_pipeline();
};

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32