#pragma once
#include "core/defines.h"
#include "core/logger.h"
#include "fx.h"


namespace gravity {
namespace renderer {
namespace dx12 {

/// @brief Assertion that DirectX12 API call ended with success
/// @param hr result of call
/// @param err error message for failure
inline void DX_ASSERT(HRESULT hr, const char* err) {
    if (FAILED(hr)) {
        core::logger::Logger::get()->fatal("DirectX Function failed: |%s|", err);
        std::exit(1);
    }
}

/// @brief Debug controller interface for debug layer in DX12
struct DebugController {
    ComPtr<ID3D12Debug4> controller;

    /// @brief Create a new controller for the debug layer
    /// @return a valid debug controller
    static DebugController create() {
        ComPtr<ID3D12Debug4> controller;
        DX_ASSERT(
            D3D12GetDebugInterface(IID_PPV_ARGS(&controller))
            , "Failed to create debug controller"
        );

        DebugController rv;
        rv.controller = controller;

        return rv;
    }

    /// @brief Enable the debug layer
    void enable() {
        // TODO: add guards for DEBUG build
        controller->EnableDebugLayer();
    }

    UINT get_debug_flag() {
        return DXGI_CREATE_FACTORY_DEBUG;
    }
};

struct Adapter {
    WCHAR* description;
    SIZE_T dedicated_video_memory { 0 };
    ComPtr<IDXGIAdapter4> adapter {};
    
    static Adapter create(D3D_FEATURE_LEVEL minimum_capability) {
        SIZE_T max_dedicated_video_memory = 0;
        ComPtr<IDXGIFactory6> dxgi_factory;
        DX_ASSERT(
            CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory))
            , "Failed to create Factory"
        );

        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIAdapter1> best_adapter = nullptr;
        DXGI_ADAPTER_DESC1 best_desc;
        for (
            UINT i = 0; 
            dxgi_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND;
            i++
        ) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // TODO: Support software/warp adapters ?
            if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), minimum_capability, __uuidof(ID3D12Device), nullptr))) {
                core::logger::Logger::get()->debug("Adapter %s:", desc.Description);
                core::logger::Logger::get()->debug("    Video Memory %zu", desc.DedicatedVideoMemory);
                if (desc.DedicatedVideoMemory > max_dedicated_video_memory) {
                    best_adapter = adapter;
                    max_dedicated_video_memory = desc.DedicatedVideoMemory;
                    best_desc = desc;
                }

                ComPtr<ID3D12Device10> device;
                DX_ASSERT(
                    D3D12CreateDevice(adapter.Get(), minimum_capability, IID_PPV_ARGS(&device)),
                    "Adapter failed to create device."
                );
            } else {
                core::logger::Logger::get()->error("Incompatible adapter %s", desc.Description);
            }
        }

        if (!best_adapter) {
            core::logger::Logger::get()->fatal("No compatible adapters found.");
            exit(1);
        }

        core::logger::Logger::get()->debug("Adapter chosen %s. [Video Memory: %zu]", best_desc.Description, best_desc.DedicatedVideoMemory);

        ComPtr<IDXGIAdapter4> adapter4;
        DX_ASSERT(
            best_adapter.As(&adapter4),
            "Unable to convert adapter to adapter4."
        );

        Adapter rv;
        rv.adapter = adapter4;
        rv.dedicated_video_memory = best_desc.DedicatedVideoMemory;
        rv.description = best_desc.Description;

        return rv;
    }
};

struct Device {
    ComPtr<ID3D12Device> device;

    /// @brief Create a logical device
    /// @param factory Factory to create the device
    /// @param warp True if we want to use warp device. False otherwise
    /// @return The device wrapper
    static Device create(ComPtr<IDXGIFactory4> factory, bool warp = false) {
        Device rv;
        
        if (warp) {
            ComPtr<IDXGIAdapter> warp_adapter;
            DX_ASSERT(
                factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter))
                , "DX12_Renderer::_load_pipeline -> EnumWarpAdapter failed"
            );

            DX_ASSERT(
                D3D12CreateDevice(
                    warp_adapter.Get()
                    ,D3D_FEATURE_LEVEL_11_0
                    ,IID_PPV_ARGS(&rv.device)
                )
                , "DX12_Renderer::_load_pipeline -> Failed to create warp device"
            );
        } else {
            ComPtr<IDXGIAdapter1> hardware_adapter;
            get_hardware_adapter(factory.Get(), &hardware_adapter);

            DX_ASSERT(
                D3D12CreateDevice(
                    hardware_adapter.Get()
                    , D3D_FEATURE_LEVEL_11_0
                    , IID_PPV_ARGS(&rv.device)
                )
                , "Device::create -> failed to create hardware device"
            );
        }

        return rv;
    }

    /// @brief Get the rtv descriptor size
    /// @return the rtv descriptor size
    UINT descriptor_handle_incriment_size() const {
        return device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    /// @brief Get the desired hardware adapter
    /// @param p_factory Factory to create the adapter
    /// @param pp_adapter The adapter we are creating
    /// @param requesting_high_performance Whether we want to specify high performance adapter
    static void get_hardware_adapter(
        IDXGIFactory4* p_factory
        , IDXGIAdapter1** pp_adapter
        , bool requesting_high_performance = true
    ) {
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

    /// @brief Create a render target view from the device and the handle
    /// @param resource The resource to create the rtv
    /// @param rtv_handle The descriptor handle
    /// @param desc The description of the rtv. NULL by default
    void create_rtv(
        ComPtr<ID3D12Resource> resource
        , D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle
        , D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr
    ) {
        device->CreateRenderTargetView(resource.Get(), desc, rtv_handle);
    }
};

struct CommandQueue {
    ComPtr<ID3D12CommandQueue> command_queue;

    /// @brief Create a command queue
    /// @param device Device to create the command queue
    /// @param flags Flags for the queue description
    /// @param type The type of command list
    /// @return 
    static CommandQueue create(
        const Device& device
        , D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE
        , D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT
    ) {
        CommandQueue rv {};
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = flags;
        desc.Type = type;


        DX_ASSERT(
            device.device->CreateCommandQueue(
                &desc
                , IID_PPV_ARGS(&rv.command_queue)
            )
            , "CommandQueue::create -> Failed to create command queue"
        );

        return rv;
    }
};

struct SwapChain {
    ComPtr<IDXGISwapChain4> swapchain;

    /// @brief Create a new swapchain struct
    /// @param factory The factory to create the swapchain
    /// @param command_queue The command queue for the swapchain
    /// @param width Width to create the swapchain with
    /// @param height Height to create the swapchain with
    /// @param hwnd The hwindow reference to give the swapchain
    /// @return The swapchain struct
    static SwapChain create(
        ComPtr<IDXGIFactory4> factory
        , const CommandQueue& command_queue
        , u32 width
        , u32 height
        , HWND hwnd
    ) {
        SwapChain rv;
        ComPtr<IDXGISwapChain1> sc;

        DXGI_SWAP_CHAIN_DESC1 desc {};
        desc.BufferCount = NUM_FRAMES;
        desc.Width = width;
        desc.Height = height;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = 1;

        DX_ASSERT(
            factory->CreateSwapChainForHwnd(
                command_queue.command_queue.Get()
                , hwnd
                , &desc
                , nullptr
                , nullptr 
                , &sc
            )
            , "Swapchain::create -> Failed to create swapchain"
        );

        DX_ASSERT(
            sc.As(&rv.swapchain)
            , "SwapChain::create -> Unable to cast to SwapChain4"
        );

        return rv;
    }

    /// @brief Create a buffer from the swapchian
    /// @param buffer_index The index of the buffer we want to create
    /// @param resource The resource that this buffer will use
    void create_buffer(
        UINT buffer_index
        , ComPtr<ID3D12Resource> resource
    ) {
        DX_ASSERT(
            swapchain->GetBuffer(buffer_index, IID_PPV_ARGS(&resource))
            , "SwapChain::create_buffer -> Unable to create buffer"
        );
    }

    /// @brief Get the current backbuffer index
    /// @return The backbuffer index
    UINT backbuffer_index() {
        return  swapchain->GetCurrentBackBufferIndex();
    }
};

struct DescriptorHeap {
    ComPtr<ID3D12DescriptorHeap> heap;

    static DescriptorHeap create(
        const Device& device
    ) {
        DescriptorHeap rv = {};
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = NUM_FRAMES;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        DX_ASSERT(
            device.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rv.heap))
            , "DescriptorHeap::create -> Unable to create descriptor heap"
        );

        return rv;
    }

    /// @brief Get the CPU descriptor handle
    /// @return The CPU descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle_for_start() {
        return heap->GetCPUDescriptorHandleForHeapStart();
    }
};

struct CommandAllocator {
    // ComPtr<ID3D12CommandAllocator> allocator;

    static CommandAllocator create(
        const Device& device
        , D3D12_COMMAND_LIST_TYPE type
    ) {
        CommandAllocator rv = {};
        ComPtr<ID3D12CommandAllocator> allocator;
        DX_ASSERT(
            device.device->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator))
            , "CommandAllocator::create -> Unable to create command allocator"
        );

        return rv;
    }
};

} // dx12 namespace
} // renderer namespace
} // gravity namespace