#include "renderer/dx12/renderer.h"

#if defined(Q_PLATFORM_WINDOWS)

#define NOMINMAX
#include <Windows.h>

#include "core/logger.h"

namespace gravity {
namespace renderer {
namespace dx12 {

using namespace core::logger;

/// @brief Initialize DirectX12 Renderer backend
/// @param conf Configuration of the renderer
/// @return True if successful. False otherwise
bool DX12_Renderer::startup(const config& conf) {
    Logger::get()->info("Initializing DirectX12 Renderer...");
    _state.frame_index = 0;
    _state.current_height = conf.height;
    _state.current_width = conf.width;
    _state.hwnd = conf.window_handle.hwindow;
    
    _viewport = CD3DX12_VIEWPORT(
        0.0f                           // topleftx
        ,0.0f                          // toplefty
        ,static_cast<f32>(conf.width)  // width
        ,static_cast<f32>(conf.height) // height
    );

    _scissor_rect = CD3DX12_RECT(
        0
        ,0
        ,static_cast<LONG>(conf.width)
        ,static_cast<LONG>(conf.height)
    );

    _rtv_descriptor_size = 0;

    _load_pipeline();

    return true;
}

/// @brief Shutdown DX12 Renderer
void DX12_Renderer::shutdown() {
    Logger::get()->debug("DX12 Renderer shutdown.");
    // _flush(
    //     _command_queue,
    //     _fence,
    //     _fence_value,
    //     _fence_event
    // );
}

void DX12_Renderer::begin_frame() {

}

void DX12_Renderer::end_frame() {

}

void DX12_Renderer::present() {
}

void DX12_Renderer::resize(u32 width, u32 height) {
    // if (_client_width != width || _client_height != height) {
    //     // do not allow for swapchain back bufferes of 0 size
    //     _client_width = std::max<u32>(1, width);
    //     _client_height = std::max<u32>(1, height);

    //     _flush(_command_queue, _fence, _fence_value, _fence_event);

    //     for (i32 i = 0; i < NUM_FRAMES; i++) {
    //         _backbuffers[i].Reset();
    //         _frame_fence_values[i] = _frame_fence_values[_current_back_buffer_index];
    //     }

    //     DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    //     DX_ASSERT(_swapchain->GetDesc(&swap_chain_desc));
    //     DX_ASSERT(_swapchain->ResizeBuffers(
    //         NUM_FRAMES, _client_width, 
    //         _client_height, 
    //         swap_chain_desc.BufferDesc.Format, 
    //         swap_chain_desc.Flags
    //         )
    //     );

    //     _current_back_buffer_index = _swapchain->GetCurrentBackBufferIndex();
    //     _update_render_target_views(_device, _swapchain, _rtv_descriptor_heap);
    // }
}

/// @brief Load objects necessary to create our pipeline
void DX12_Renderer::_load_pipeline() {
    UINT dxgi_factory_flags { 0 };
    DebugController debug_controller = DebugController::create();

#if defined(_DEBUG)
    debug_controller.enable();
    dxgi_factory_flags |= debug_controller.get_debug_flag();
#endif // _DEBUG

    ComPtr<IDXGIFactory4> factory;
    DX_ASSERT(
        CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory))
        , "DX12_Renderer::_load_pipeline failed to create factory 4"
    );

    Device device = Device::create(factory, _use_warp);
    CommandQueue command_queue= CommandQueue::create(device);
    SwapChain swapchain = SwapChain::create(
        factory
        , command_queue
        , _state.current_width
        , _state.current_height
        , _state.hwnd
    );

    _state.frame_index = swapchain.backbuffer_index();
    DescriptorHeap descriptor_heap = DescriptorHeap::create(device);
    _rtv_descriptor_size = device.descriptor_handle_incriment_size();

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(descriptor_heap.cpu_descriptor_handle_for_start());
    for (UINT i = 0; i < NUM_FRAMES; ++i) {
        swapchain.create_buffer(i, _render_targets[i]);
        device.create_rtv(_render_targets[i], rtv_handle);
        rtv_handle.Offset(1, _rtv_descriptor_size);
    }
}

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32