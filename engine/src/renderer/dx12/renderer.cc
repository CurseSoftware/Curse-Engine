#include "renderer/dx12/renderer.h"

#if defined(Q_PLATFORM_WINDOWS)

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

    _enable_debug_layer();
    auto adapter = _get_adapter(false);
    _device = _create_device(adapter);


    Logger::get()->info("DirectX12 Renderer initialized successfully.");
    return true;
}

/// @brief Shutdown DX12 Renderer
void DX12_Renderer::shutdown() {
    Logger::get()->debug("DX12 Renderer shutdown.");
}

void DX12_Renderer::begin_frame() {

}

void DX12_Renderer::end_frame() {

}

void DX12_Renderer::present() {

}

/// @brief Enable debug layer
void DX12_Renderer::_enable_debug_layer() {
    DX_ASSERT(
        D3D12GetDebugInterface(IID_PPV_ARGS(&_debug_interface))
    );
    _debug_interface->EnableDebugLayer();
    core::logger::Logger::get()->debug("DX12 Debug Layer Enabled");
}

/// @brief Get an adapter that can create the device that we want
/// @param use_warp Whether we want to use WARP adapter
/// @return The adapter that is suitable for creating the device
ComPtr<IDXGIAdapter4> DX12_Renderer::_get_adapter(bool use_warp) {
    ComPtr<IDXGIFactory4> dxgi_factory;

    UINT create_factory_flags = 0;

    #if defined(_DEBUG)
    create_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
    #endif

    DX_ASSERT(
        CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(&dxgi_factory))
    );

    ComPtr<IDXGIAdapter1> dxgi_adapter1;
    ComPtr<IDXGIAdapter4> dxgi_adapter4;

    if (use_warp) {
        DX_ASSERT(
            dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(&dxgi_adapter1))
        );
        DX_ASSERT(
            dxgi_adapter1.As(&dxgi_adapter4)
        );
    } else {
        SIZE_T max_dedicated_video_memory = 0;

        for (UINT i = 0; dxgi_factory->EnumAdapters1(i, &dxgi_adapter1) != DXGI_ERROR_NOT_FOUND; i++) {
            DXGI_ADAPTER_DESC1 dxgi_adapter_desc1;
            dxgi_adapter1->GetDesc1(&dxgi_adapter_desc1);

            // See if we can create a device without actually creating one
            // in order to see if this works
            if ((dxgi_adapter_desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgi_adapter1.Get(), 
                    D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) && 
                dxgi_adapter_desc1.DedicatedVideoMemory > max_dedicated_video_memory)
            {
                max_dedicated_video_memory = dxgi_adapter_desc1.DedicatedVideoMemory;
                DX_ASSERT(dxgi_adapter1.As(&dxgi_adapter4));
            }
        }
    }

    core::logger::Logger::get()->debug("DX12 Adapter created.");
    return dxgi_adapter4;
}

/// @brief Create the dx12 device
/// @param adapter The adapter that we want to create the device on
/// @return The device
ComPtr<ID3D12Device2> DX12_Renderer::_create_device(ComPtr<IDXGIAdapter4> adapter) {
    ComPtr<ID3D12Device2> device2;
    DX_ASSERT(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device2)));

    #if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> p_info_queue;

    if (SUCCEEDED(device2.As(&p_info_queue))) {
        p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        // Suppress categories of msgs
        // D3D12_MESSAGE_CATEGORY categories[] = {};

        // Suppress messages with this level
        D3D12_MESSAGE_SEVERITY severities[] = {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // Suppress messages by their ID
        D3D12_MESSAGE_ID deny_ids[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
            ,D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE
            ,D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
        };

        D3D12_INFO_QUEUE_FILTER new_filter = {};
        // new_filter.DenyList.NumCategories = _countof(categories);
        // new_filter.DenyList.pCategorylist = categories;
        new_filter.DenyList.NumSeverities = _countof(severities);
        new_filter.DenyList.pSeverityList = severities;
        new_filter.DenyList.NumIDs = _countof(deny_ids);
        new_filter.DenyList.pIDList = deny_ids;

        DX_ASSERT(
            p_info_queue->PushStorageFilter(&new_filter)
        );
    }
    #endif // _DEBUG

    core::logger::Logger::get()->debug("DX12 Device Created");
    return device2;
}

/// @brief Create a command queue from the device with a desired type
/// @param device The device to create the queue
/// @param type The type of queue to create
/// @return The command queue
ComPtr<ID3D12CommandQueue> DX12_Renderer::_create_command_queue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) {
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ComPtr<ID3D12CommandQueue> command_queue;
    DX_ASSERT(
        device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue))
    );

    core::logger::Logger::get()->debug("DX12 Command Queue Created");
    return command_queue;
}

/// @brief Enabling tearing allows for vscync to be disabled. This allows for faster presentation 
/// @return True if tearing is allowed and false if not.
bool DX12_Renderer::_check_treat_support() {
    BOOL allow_tearing = FALSE;

    ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))) {
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory4.As(&factory5))) {
            if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                &allow_tearing,
                sizeof(allow_tearing)
            ))) {
                allow_tearing = FALSE;
            }
        }
    }

    return allow_tearing == TRUE;
}

ComPtr<IDXGISwapChain4> DX12_Renderer::_create_swapchain(
    HWND hwnd,
    ComPtr<ID3D12CommandQueue> command_queue,
    u32 width, u32 height,
    u32 n_buffers
) {
    ComPtr<IDXGISwapChain4> swapchain4;
    ComPtr<IDXGIFactory4> factory4;

    UINT create_factory_flags = 0;

    #if defined (_DEBUG)
    create_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
    #endif // _DEBUG

    DX_ASSERT(
        CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(&factory4))
    );

    DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
    swapchain_desc.Width = width;
    swapchain_desc.Height = height;
    swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.Stereo = FALSE;
    swapchain_desc.SampleDesc = { 1, 0 };
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapchain_desc.Flags = _check_treat_support() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapchain1;
    DX_ASSERT(
        factory4->CreateSwapChainForHwnd(
            command_queue.Get(),
            hwnd,
            &swapchain_desc,
            nullptr,
            nullptr,
            &swapchain1
        )
    );

    DX_ASSERT(
        factory4->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)
    );

    DX_ASSERT(
        swapchain1.As(&swapchain4)
    );

    return swapchain4;
}
    
ComPtr<ID3D12DescriptorHeap> DX12_Renderer::_create_descriptor_head(
    ComPtr<ID3D12Device2> device, 
    D3D12_DESCRIPTOR_HEAP_TYPE type, 
    u32 n_descs
) {
    ComPtr<ID3D12DescriptorHeap> descriptor_heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = n_descs;
    desc.Type = type;

    DX_ASSERT(
        device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_heap))
    );

    return descriptor_heap;
}

// void DX12_Renderer::_update_render_targets(
//     ComPtr<ID3D12Device2> device, 
//     ComPtr<IDXGISwapChain4> swapchain, 
//     ComPtr<ID3D12DescriptorHeap> descriptor_heap
// ) {
//     auto rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    
// }

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32