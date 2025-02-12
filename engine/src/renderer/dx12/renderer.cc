#include "renderer/dx12/renderer.h"
#include "renderer/dx12/dx12_types.h"

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
bool DX12Renderer::startup(const config& conf) {
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

    u32 dxgi_flags = 0;

#if defined(Q_DEBUG)
    {
        ComPtr<ID3D12Debug4> debug_interface;
        DX_ASSERT(
            D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface))
            , "Failed to create debug interface"
        );
        debug_interface->EnableDebugLayer();
        dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif // Q_DEBUG

    DX_ASSERT(CreateDXGIFactory2(dxgi_flags, IID_PPV_ARGS(&_factory)), "Failed to create factory.");
    ComPtr<IDXGIAdapter4> main_adapter;
    main_adapter.Attach(_determine_main_adapter());
    if (!main_adapter) {
        Logger::get()->fatal("Failed to create DXGI Adapter.");
        return false;
    }

    D3D_FEATURE_LEVEL max_feature_level { _get_max_feature_level(main_adapter.Get()) };

    DX_ASSERT(
        D3D12CreateDevice(main_adapter.Get(), max_feature_level, IID_PPV_ARGS(&_device))
        , "Unable to create device for renderer."
    );

    NAME_DX12_OBJ(_device, L"MAIN DEVICE");

    assert(_rtv_desc_heap.initialize(_device.Get(), 512, false, this) && "Failed to initialize rtv heap");
    assert(_dsv_desc_heap.initialize(_device.Get(), 512, false, this) && "Failed to initialize dsv heap");
    assert(_srv_desc_heap.initialize(_device.Get(), 4096, true, this) && "Failed to initialize srv heap");
    assert(_uav_desc_heap.initialize(_device.Get(), 512, false, this) && "Failed to initialize uav heap");

#if defined(Q_DEBUG)
    {
        Logger::get()->trace("DEBUG");
        ComPtr<ID3D12InfoQueue> info_queue;
        DX_ASSERT(_device->QueryInterface(IID_PPV_ARGS(&info_queue)), "Unable to query info queue");

        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
    }
#endif // Q_DEBUG

    new (&_gfx_command) DXCommand(_device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
    if (!_gfx_command.queue())
    {
        Logger::get()->fatal("Unable to initialize DXCommand");
        return false;
    }
    
    return true;
}

/// @brief Get the maximum feature level supported
/// @param adapter
/// @return the feature level
D3D_FEATURE_LEVEL DX12Renderer::_get_max_feature_level(IDXGIAdapter4* adapter) {
    constexpr D3D_FEATURE_LEVEL levels[4] = {
        D3D_FEATURE_LEVEL_11_0
        , D3D_FEATURE_LEVEL_11_1
        , D3D_FEATURE_LEVEL_12_0
        , D3D_FEATURE_LEVEL_12_1
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS feature_level_info {};
    feature_level_info.NumFeatureLevels = _countof(levels);
    feature_level_info.pFeatureLevelsRequested = levels;

    ComPtr<ID3D12Device> device;
    DX_ASSERT(
        D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))
        , "Failed to create device with minimum feature level."
    );
    DX_ASSERT(
        device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &feature_level_info, sizeof(feature_level_info))
        , "Device is unable to support feature level"
    );
    
    return feature_level_info.MaxSupportedFeatureLevel;
}

IDXGIAdapter4* DX12Renderer::_determine_main_adapter() {
    IDXGIAdapter4* adapter { nullptr };
    for (
        u32 i = 0
        ; _factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND
        ; ++i
    ) {
        if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) {
            return adapter;
        }
        release(adapter);
    }

    return nullptr;
}

/// @brief Shutdown DX12 Renderer
void DX12Renderer::shutdown() {
    Logger::get()->debug("DX12 Renderer shutdown.");


    
    _gfx_command.release();
    _rtv_desc_heap.release();
    _dsv_desc_heap.release();
    _srv_desc_heap.release();
    _uav_desc_heap.release();

#if defined(Q_DEBUG)
    {
        ComPtr<ID3D12InfoQueue> info_queue;
        DX_ASSERT(_device->QueryInterface(IID_PPV_ARGS(&info_queue)), "Unable to query info queue");

        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);

        ComPtr<ID3D12DebugDevice2> debug_device;
        DX_ASSERT(
            _device->QueryInterface(IID_PPV_ARGS(&debug_device))
            , "Failed to create debug device on shutdown"
        );

        DX_ASSERT(debug_device->ReportLiveDeviceObjects(
                D3D12_RLDO_SUMMARY
                | D3D12_RLDO_DETAIL
                | D3D12_RLDO_IGNORE_INTERNAL
            )
            , "Failed to report live objects."
        );
    }
#endif // Q_DEBUG
}

void DX12Renderer::begin_frame() {
    
}

void DX12Renderer::end_frame() {

}

void DX12Renderer::present() {
    _gfx_command.begin_frame();
    ID3D12GraphicsCommandList6* list = _gfx_command.list();


    _gfx_command.end_frame();
}

void DX12Renderer::set_deferred_releases_flag() {
    _deferred_release_flags[frame_index()] = 1;
}

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32
