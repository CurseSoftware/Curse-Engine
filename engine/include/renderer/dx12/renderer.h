#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "core/logger.h"
#include "renderer/renderer.h"
#include "renderer/dx12/dx12_types.h"
#include "renderer/dx12/resources.h"
#include "fx.h"
#include "dx.h"
#include <chrono>
#include <queue>

#if defined(Q_PLATFORM_WINDOWS)
#include <DirectXMath.h>

namespace gravity {
namespace renderer {
namespace dx12 {

class DXCommand {
public:
    DXCommand() = default;
    DISABLE_COPY_AND_MOVE(DXCommand);
    explicit DXCommand(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type);
    ~DXCommand();

    void begin_frame();
    void end_frame();
    void release();

    constexpr ID3D12CommandQueue *const queue() const { return _queue; }
    constexpr ID3D12GraphicsCommandList6 *const list() const { return _list; }
    constexpr u32 frame_index() const { return _frame_index; }

private:
    struct command_frame {
        u64 fence_value { 0 };
        ID3D12CommandAllocator* allocator { nullptr };

        void release()
        {
            dx12::release(allocator);
        }

        void wait(HANDLE fence_event, ID3D12Fence1* fence)
        {
            assert(fence && fence_event);
            if (fence->GetCompletedValue() < fence_value)
            {
                DX_ASSERT(fence->SetEventOnCompletion(fence_value, fence_event)
                    , "command_frame Unable to set event"
                );
                WaitForSingleObject(fence_event, INFINITE);
            }
        }
    };

    void _flush();

    ID3D12CommandQueue* _queue { nullptr };
    ID3D12GraphicsCommandList6* _list { nullptr };
    command_frame _cmd_frames[NUM_FRAMES];
    ID3D12Fence1* _fence { nullptr };
    HANDLE _fence_event { nullptr };
    u64 _fence_value { 0 };
    u32 _frame_index { 0 };
};

class DX12Renderer : public Renderer {
public:
    bool startup(const config& conf) override;
    void shutdown() override;

    void begin_frame() override;
    void end_frame() override;
    void present() override;
    
    void set_deferred_releases_flag();

    void resize(u32 width, u32 height);
    u32 frame_index() const { return _gfx_command.frame_index(); };
private:
    UINT _use_warp { false };

    CD3DX12_VIEWPORT _viewport;
    CD3DX12_RECT _scissor_rect;
    ComPtr<IDXGISwapChain3> _swapchain;
    ComPtr<ID3D12Device8> _device;
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
    ComPtr<IDXGIFactory7> _factory;

    u32 _deferred_release_flags[NUM_FRAMES] {};
    DXCommand _gfx_command;
    DXDescriptorHeap _rtv_desc_heap  { D3D12_DESCRIPTOR_HEAP_TYPE_RTV };
    DXDescriptorHeap _dsv_desc_heap  { D3D12_DESCRIPTOR_HEAP_TYPE_DSV };
    DXDescriptorHeap _srv_desc_heap  { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };
    DXDescriptorHeap _uav_desc_heap  { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };
    std::vector<IUnknown*> _deferred_releases[NUM_FRAMES];
    std::mutex _deferred_release_mutex;

    UINT curr_list = 0;

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
    IDXGIAdapter4* _determine_main_adapter();
    D3D_FEATURE_LEVEL _get_max_feature_level(IDXGIAdapter4* adapter);
};

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32