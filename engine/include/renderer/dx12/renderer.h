#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "core/logger.h"
#include "renderer/renderer.h"
#include "fx.h"
#include "dx.h"
#include <chrono>

#if defined(Q_PLATFORM_WINDOWS)
#include <DirectXMath.h>

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
    ComPtr<ID3D12CommandAllocator> _bundle_allocator;
    ComPtr<ID3D12CommandQueue> _command_queue;
    ComPtr<ID3D12RootSignature> _root_signature;
    ComPtr<ID3D12DescriptorHeap> _rtv_heap;
    ComPtr<ID3D12DescriptorHeap> _cbv_heap;
    ComPtr<ID3D12PipelineState> _pipeline_state;
    ComPtr<ID3D12GraphicsCommandList> _command_list;
    ComPtr<ID3D12GraphicsCommandList> _bundle;
    UINT _rtv_descriptor_size;

    HANDLE _fence_event;
    ComPtr<ID3D12Fence> _fence;

    struct {
        u32 current_width;
        u32 current_height;
        UINT frame_index;
        UINT64 fence_value;
        HWND hwnd;
    } _state;

    struct Vertex {
        DirectX::XMFLOAT3  position;
        DirectX::XMFLOAT4  color;
    };

    struct SceneConstantBuffer {
        DirectX::XMFLOAT4  offset;
        float padding[60];
    };
    static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

    // TEMP
    ComPtr<ID3D12Resource> _vertex_buffer;
    ComPtr<ID3D12Resource> _constant_buffer;
    SceneConstantBuffer _constant_buffer_data;
    UINT8* _p_cbv_data_begin;
    D3D12_VERTEX_BUFFER_VIEW _vertex_buffer_view;

    /* METHODS */
    void _load_pipeline();
    void _load_assets();
    void _populate_command_buffers();
    void _wait_for_prev_frame();
};

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32