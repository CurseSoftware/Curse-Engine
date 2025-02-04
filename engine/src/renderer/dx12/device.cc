// #include "core/defines.h"
#include "renderer/dx12/dx12_types.h"

#if defined(Q_PLATFORM_WINDOWS)

namespace gravity {
namespace renderer {
namespace dx12 {
using namespace core::logger;

DX12Device::DX12Device(bool use_warp) {
    UINT dxgi_factory_flags = 0;

#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debug_controller;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
            debug_controller->EnableDebugLayer();
            dxgi_factory_flags|= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
    Logger::get()->debug("DirectX12 Debug enabled.");
#endif // _DEBUG
    
    // ComPtr<IDXGIFactory4> factory;
    DX_ASSERT(
        CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&_factory))
        , "DX12Device -> Failed to create factory"
    );

    Logger::get()->debug("Factory created.");

    if (use_warp) {
        ComPtr<IDXGIAdapter4> warp_adapter;
        DX_ASSERT(_factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)), "DX12_Renderer::_load_pipeline -> Failed to enumerate warp adapter");
        DX_ASSERT(
            D3D12CreateDevice(
                warp_adapter.Get()
                , D3D_FEATURE_LEVEL_11_0
                , IID_PPV_ARGS(&_device)
            )
            , "DX12Device -> Failed to create warp device"
        );
        Logger::get()->debug("Warp device created.");
    } else {
        ComPtr<IDXGIAdapter1> hardware_adapter;
        _get_hardware_adapter(_factory.Get(), &hardware_adapter);

        DX_ASSERT(
            D3D12CreateDevice(
                hardware_adapter.Get()
                , D3D_FEATURE_LEVEL_11_0
                , IID_PPV_ARGS(&_device)
            )
            , "DX12Device -> Failed to create hardware device"
        );
        Logger::get()->debug("Hardware device created.");
    }

    // Create command queue
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        DX_ASSERT(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_command_queue))
            , "DX12Device -> Failed to create command queue"
        );
        Logger::get()->debug("Command queue created successfully.");
    }
}

void DX12Device::_get_hardware_adapter(IDXGIFactory4* p_factory, IDXGIAdapter1** pp_adapter, bool requesting_high_performance) {
        *pp_adapter = nullptr;

        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIFactory6> factory6;

        if (SUCCEEDED(p_factory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
            for (
                UINT idx = 0
                ; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                    idx
                    , requesting_high_performance 
                        ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
                        : DXGI_GPU_PREFERENCE_UNSPECIFIED
                    , IID_PPV_ARGS(&adapter)
                ))
                ; ++idx
            ) {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);
                
                core::logger::Logger::get()->debug("Adapter %s:", desc.Description);
                core::logger::Logger::get()->debug("    Video Memory %zu", desc.DedicatedVideoMemory);

                if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
                    continue;
                }

                if (SUCCEEDED(
                    D3D12CreateDevice(
                        adapter.Get()
                        , D3D_FEATURE_LEVEL_11_0
                        , __uuidof(ID3D12Device)
                        , nullptr
                    )
                )) {
                    break;
                }
            }
        }

        if (adapter.Get() == nullptr) {
            for (
                UINT idx = 0
                ; SUCCEEDED(p_factory->EnumAdapters1(idx, &adapter))
                ; ++idx
            ) {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);
                
                core::logger::Logger::get()->debug("Adapter %s:", desc.Description);
                core::logger::Logger::get()->debug("    Video Memory %zu", desc.DedicatedVideoMemory);

                if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
                    continue;
                }

                if (SUCCEEDED(
                    D3D12CreateDevice(
                        adapter.Get()
                        , D3D_FEATURE_LEVEL_11_0
                        , __uuidof(ID3D12Device)
                        , nullptr
                    )
                )) {
                    break;
                }
            }
        }

        *pp_adapter = adapter.Detach();
}

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WINDOWS