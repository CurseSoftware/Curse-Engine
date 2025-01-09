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

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32