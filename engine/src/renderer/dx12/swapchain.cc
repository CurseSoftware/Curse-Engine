#include "renderer/dx12/dx12_types.h"

#if defined(Q_PLATFORM_WINDOWS)

namespace gravity {
namespace renderer {
namespace dx12 {

DX12SwapChain::DX12SwapChain(DX12Device* device, HWND hwnd, UINT width, UINT height) {
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors = NUM_FRAMES;
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    DX_ASSERT(device->device()->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&_rtv_heap))
        , "DX12SwapChain -> Failed to create rtv heap"
    );
    _rtv_descriptor_size = device->device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    Logger::get()->debug("SwapChain descriptor heap created successfully.");

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.BufferCount = NUM_FRAMES;
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapchain;
    DX_ASSERT(device->factory()->CreateSwapChainForHwnd(
            device->command_queue()
            , hwnd
            , &desc
            , nullptr
            , nullptr
            , &swapchain
        )
        , "DX12SwapChain -> unable to create SwapChain1"
    );

    Logger::get()->debug("SwapChain interface created successfully.");

    DX_ASSERT(device->factory()->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)
        , "DX12SwapChain -> unable to make window association"
    );
    Logger::get()->debug("SwapChain window association made.");

    DX_ASSERT(swapchain.As(&_swapchain), "DX12SwapChain -> Unable to cast SwapChain1 to SwapChain3");
    _current_frame_index = _swapchain->GetCurrentBackBufferIndex();

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(_rtv_heap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < NUM_FRAMES; i++) {
        DX_ASSERT(_swapchain->GetBuffer(i, IID_PPV_ARGS(&_render_targets[i]))
            , "DX12SwapChain -> Failed to create buffer"
        );
        rtv_handle.Offset(1, _rtv_descriptor_size);
    }
    
    Logger::get()->debug("SwapChain created successfully.");
}

void DX12SwapChain::present() {
    DX_ASSERT(_swapchain->Present(1, 0), "DX12SwapChain -> Failed to present");
    _current_frame_index = _swapchain->GetCurrentBackBufferIndex();
}

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif