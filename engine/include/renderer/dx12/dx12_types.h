#pragma once
#include "renderer/renderer.h"
#include "renderer/dx12/fx.h"
#include "renderer/dx12/dx.h"
// #include "renderer/dx12/renderer.h"

#if defined(Q_PLATFORM_WINDOWS)

namespace gravity {
namespace renderer {
namespace dx12 {

class DX12CommandList;

class DX12Device {
public:
    DX12Device(bool use_warp = false);

    ID3D12Device* device() const { return _device.Get(); }
    ID3D12CommandQueue* command_queue() const { return _command_queue.Get(); }
    IDXGIFactory4* factory() const { return _factory.Get(); }

    void submit_direct(DX12CommandList *list);

private:
    ComPtr<ID3D12Device> _device;
    ComPtr<ID3D12CommandQueue> _command_queue;
    ComPtr<IDXGIFactory4> _factory;

    void _get_hardware_adapter(IDXGIFactory4* factory, IDXGIAdapter1** pp_adapter, bool high_perf = true);
};

class DX12SwapChain {
public:
    DX12SwapChain(DX12Device *device, HWND hwnd, UINT width, UINT height);
    void present();

    ID3D12Resource* current_render_target() const { return _render_targets[_current_frame_index].Get(); }
    UINT current_backbuffer_index() const { return _current_frame_index; }

private:
    ComPtr<IDXGISwapChain3> _swapchain;
    ComPtr<ID3D12DescriptorHeap> _rtv_heap;
    ComPtr<ID3D12Resource> _render_targets[NUM_FRAMES];
    UINT _rtv_descriptor_size;
    UINT _current_frame_index;
};

class DX12CommandList {
public:
    DX12CommandList(DX12Device* device);
    void reset();

    ID3D12GraphicsCommandList* get() const { return _command_list.Get(); }

private:
    ComPtr<ID3D12CommandAllocator> _allocator;
    ComPtr<ID3D12GraphicsCommandList> _command_list;
};

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WINDOWS