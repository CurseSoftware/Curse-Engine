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
    // _load_assets();

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
//     UINT dxgi_factory_flags = 0;

// #ifdef _DEBUG
//     {
//         ComPtr<ID3D12Debug> debug_controller;
//         if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
//             debug_controller->EnableDebugLayer();
//             dxgi_factory_flags|= DXGI_CREATE_FACTORY_DEBUG;
//         }
//     }
// #endif // _DEBUG

//     ComPtr<IDXGIFactory4> factory;
//     DX_ASSERT(
//         CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory))
//         , "DX12_Renderer::_load_pipeline -> Failed to create factory"
//     );

//     Logger::get()->debug("Factory created.");

//     if (_use_warp) {
//         ComPtr<IDXGIAdapter4> warp_adapter;
//         DX_ASSERT(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)), "DX12_Renderer::_load_pipeline -> Failed to enumerate warp adapter");
//         DX_ASSERT(
//             D3D12CreateDevice(
//                 warp_adapter.Get()
//                 , D3D_FEATURE_LEVEL_11_0
//                 , IID_PPV_ARGS(&_device)
//             )
//             , "DX12_Renderer::_load_pipeline -> Failed to create warp device"
//         );
//         Logger::get()->debug("Warp device created.");
//     } else {
//         ComPtr<IDXGIAdapter1> hardware_adapter;
//         Device::get_hardware_adapter(factory.Get(), &hardware_adapter);

//         DX_ASSERT(
//             D3D12CreateDevice(
//                 hardware_adapter.Get()
//                 , D3D_FEATURE_LEVEL_11_0
//                 , IID_PPV_ARGS(&_device)
//             )
//             , "DX12_Renderer::_load_pipeline -> Failed to create hardware device"
//         );
//         Logger::get()->debug("Hardware device created.");
//     }

//     D3D12_COMMAND_QUEUE_DESC queue_desc = {};
//     queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//     queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//     DX_ASSERT(
//         _device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&_command_queue))
//         , "DX12_Renderer::_load_pipeline -> Failed to create command queue"
//     );
//     Logger::get()->debug("Command queue created.");

//     DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
//     swapchain_desc.BufferCount = NUM_FRAMES;
//     swapchain_desc.Width = _state.current_width;
//     swapchain_desc.Height = _state.current_height;
//     swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//     swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//     swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//     swapchain_desc.SampleDesc.Count = 1;

//     ComPtr<IDXGISwapChain1> swapchain;
//     DX_ASSERT(
//         factory->CreateSwapChainForHwnd(
//             _command_queue.Get()
//             , _state.hwnd
//             , &swapchain_desc
//             , nullptr
//             , nullptr
//             , &swapchain
//         )
//         , "DX12_Renderer::_load_pipeline -> Failed to create swapchain1"
//     );

//     DX_ASSERT(
//         factory->MakeWindowAssociation(_state.hwnd, DXGI_MWA_NO_ALT_ENTER)
//         , "DX12_Renderer::_load_pipeline -> Failed to create window association"
//     );
//     DX_ASSERT(
//         swapchain.As(&_swapchain)
//         , "DX12_Renderer::_load_pipeline -> Failed to convert swapchain1 to swapchain4"
//     );
//     Logger::get()->debug("SwapChain reated.");
//     _state.frame_index = _swapchain->GetCurrentBackBufferIndex();

//     {
//         D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
//         heap_desc.NumDescriptors = NUM_FRAMES;
//         heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//         heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//         DX_ASSERT(
//             _device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&_rtv_heap))
//             , "DX12_Renderer::_load_pipeline -> Failed to create descriptor heap"
//         );

//         _rtv_descriptor_size = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//         Logger::get()->debug("Descriptor heap created.");

//         D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc {};
//         cbv_heap_desc.NumDescriptors = 1;
//         cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//         cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//         DX_ASSERT(
//             _device->CreateDescriptorHeap(
//                 &cbv_heap_desc
//                 , IID_PPV_ARGS(&_cbv_heap)
//             )
//             , "DX12_Renderer::_load_pipeline -> Failed to create constant buffer heap"
//         );
//     }

//     {
//         CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(_rtv_heap->GetCPUDescriptorHandleForHeapStart());
//         D3D12_RENDER_TARGET_VIEW_DESC *rtv_desc = nullptr;
//         for (
//             UINT idx = 0
//             ; idx < NUM_FRAMES
//             ; idx++
//         ) {
//             DX_ASSERT(
//                 _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_render_targets[idx]))
//                 , "DX12_Renderer::_load_pipeline -> Failed to create render target"
//             );
//             _device->CreateRenderTargetView(_render_targets[idx].Get(), rtv_desc, rtv_handle);
//             rtv_handle.Offset(1, _rtv_descriptor_size);
//             Logger::get()->debug("Render target view [%u] created.", idx);
//         }
//     }
//     Logger::get()->debug("Frame resources created");

//     DX_ASSERT(
//         _device->CreateCommandAllocator(
//             D3D12_COMMAND_LIST_TYPE_DIRECT
//             , IID_PPV_ARGS(&_command_allocator)
//         )
//         , "DX12_Renderer::_load_pipeline -> Failed to create command allocator"
//     );
//     Logger::get()->debug("Command list allocator created.");
    
//     DX_ASSERT(
//         _device->CreateCommandAllocator(
//             D3D12_COMMAND_LIST_TYPE_BUNDLE
//             , IID_PPV_ARGS(&_bundle_allocator)
//         )
//         , "DX12_Renderer::_load_pipeline -> Failed to create bundle allocator"
//     );
//     Logger::get()->debug("Bundle allocator created.");
    
//     Logger::get()->info("Pipeline loaded successfully.");
    DX12Device device = DX12Device();
    DX12SwapChain swapchain = DX12SwapChain(
        &device, 
        _state.hwnd, 
        _state.current_width, 
        _state.current_height
    );
}

void DX12_Renderer::_load_assets() {
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
        feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data)))) {
            feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;            
        }

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        CD3DX12_ROOT_PARAMETER1 root_parameters[1];

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

        D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags 
            = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
            | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
            | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
            | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
            | D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
        ;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
        root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, root_signature_flags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        DX_ASSERT(
            D3DX12SerializeVersionedRootSignature(&root_signature_desc, feature_data.HighestVersion, &signature, &error)
            , "DX12_Renderer::_load_pipeline -> Failed to serialize root signature"
        );
        DX_ASSERT(
            _device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_root_signature))
            , "DX12_Renderer::_load_pipeline -> Failed to create root signature"
        );

    }

    {
        ComPtr<ID3DBlob> vertex_shader;
        ComPtr<ID3DBlob> pixel_shader;

#if defined(_DEBUG)
        UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compile_flags = 0;
#endif // _DEBUG

        DX_ASSERT(
            D3DCompileFromFile(
                L"assets/shaders.hlsl"
                , nullptr
                , nullptr 
                , "VSMain"
                , "vs_5_0"
                , compile_flags
                , 0
                , &vertex_shader
                , nullptr
            )
            , "DX12_Renderer::_load_assets -> Failed to compile vertex shader"
        );
        DX_ASSERT(
            D3DCompileFromFile(
                L"assets/shaders.hlsl"
                , nullptr
                , nullptr 
                , "PSMain"
                , "ps_5_0"
                , compile_flags
                , 0
                , &pixel_shader
                , nullptr
            )
            , "DX12_Renderer::_load_assets -> Failed to compile pixel shader"
        );
        Logger::get()->info("Shaders loaded and compiled");

        D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            , { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_desc = {};
        pipeline_desc.InputLayout = { input_element_descs, _countof(input_element_descs) };
        pipeline_desc.pRootSignature = _root_signature.Get();
        pipeline_desc.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.Get());
        pipeline_desc.PS = CD3DX12_SHADER_BYTECODE(pixel_shader.Get());
        pipeline_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        pipeline_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pipeline_desc.DepthStencilState.DepthEnable = FALSE;
        pipeline_desc.DepthStencilState.StencilEnable = FALSE;
        pipeline_desc.SampleMask = UINT_MAX;
        pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipeline_desc.NumRenderTargets = 1;
        pipeline_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        pipeline_desc.SampleDesc.Count = 1;
        DX_ASSERT(
            _device->CreateGraphicsPipelineState(&pipeline_desc, IID_PPV_ARGS(&_pipeline_state))
            , "DX12_Renderer::_load_assets -> Failed to create pipeline state"
        );
        Logger::get()->info("Graphics pipeline created successfully.");
    }

    UINT node_mask = 0;
    DX_ASSERT(
        _device->CreateCommandList(node_mask, D3D12_COMMAND_LIST_TYPE_DIRECT, _command_allocator.Get(), nullptr, IID_PPV_ARGS(&_command_list))
        , "DX12_Renderer::_load_assets -> Failed to create command list"
    );

    DX_ASSERT(
        _command_list->Close()
        , "DX12_Renderer::_load_assets -> Failed to close command list"
    );

    // Vertex Buffer
    {
        f32 aspect_ratio = _state.current_width / _state.current_height;
        // Vertex triangle_vertices[] = {
        //     { 
        //         .position{ 0.0f, 0.25f * aspect_ratio, 0.0f },
        //         .color{ 1.0f, 0.2f, 0.6f, 1.0f }
        //     }
        //     ,{ 
        //         .position{ 0.25f * aspect_ratio, -0.25f * aspect_ratio, 0.0f },
        //         .color{ 0.2f, 1.0f, 0.6f, 1.0f }
        //     }
        //     ,{ 
        //         .position{ -0.25f * aspect_ratio, -0.25f * aspect_ratio, 0.0f },
        //         .color{ 0.6f, 0.6f, 1.0f, 1.0f }
        //     }
        // };
        Vertex triangle_vertices[] =
        {
            { { 0.0f, 0.25f * aspect_ratio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        const UINT vertex_buffer_size = sizeof(triangle_vertices);

        auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(vertex_buffer_size);
        DX_ASSERT(
            _device->CreateCommittedResource(
                &heap_properties
                , D3D12_HEAP_FLAG_NONE
                , &resource_desc
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&_vertex_buffer)
            )
            , "DX12_Renderer::_load_assest -> Failed to create committed resource for vertex buffer"
        );

        UINT8 *p_vertex_data_begin;
        CD3DX12_RANGE read_range(0, 0); // do not intend to read from this resource on CPU
        DX_ASSERT(_vertex_buffer->Map(0, &read_range, reinterpret_cast<void**>(&p_vertex_data_begin))
            , "DX12_Renderer::_load_assets -> Failed to map vertex buffer"
        );
        memcpy(p_vertex_data_begin, triangle_vertices, sizeof(triangle_vertices));
        _vertex_buffer->Unmap(0, nullptr);

        _vertex_buffer_view.BufferLocation = _vertex_buffer->GetGPUVirtualAddress();
        _vertex_buffer_view.StrideInBytes = sizeof(Vertex);
        _vertex_buffer_view.SizeInBytes = vertex_buffer_size;
    }

    // Create constant buffers
    {
        const UINT constant_buffer_size = sizeof(SceneConstantBuffer);
        auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(constant_buffer_size);
        DX_ASSERT(
            _device->CreateCommittedResource(
                &heap_properties
                , D3D12_HEAP_FLAG_NONE
                , &resource_desc
                , D3D12_RESOURCE_STATE_GENERIC_READ
                , nullptr
                , IID_PPV_ARGS(&_constant_buffer)
            )
            , "DX12_Renderer::_load_assets -> Failed to create constant buffer"
        );

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
        cbv_desc.BufferLocation = _constant_buffer->GetGPUVirtualAddress();
        cbv_desc.SizeInBytes = constant_buffer_size;
        _device->CreateConstantBufferView(&cbv_desc, _cbv_heap->GetCPUDescriptorHandleForHeapStart());

        CD3DX12_RANGE read_range(0, 0);
        DX_ASSERT(
            _constant_buffer->Map(0, &read_range, reinterpret_cast<void**>(&_p_cbv_data_begin))
            , "DX12_Renderer::_load_assets -> Failed to map constant buffer"
        );
        memcpy(_p_cbv_data_begin, &_constant_buffer_data, sizeof(_constant_buffer_data));
    }

    // Bundle command list
    {
        UINT mask = 0;
        DX_ASSERT(
            _device->CreateCommandList(
                mask
                , D3D12_COMMAND_LIST_TYPE_BUNDLE
                , _bundle_allocator.Get()
                , _pipeline_state.Get()
                , IID_PPV_ARGS(&_bundle)
            )
            , "DX12_Renderer::_load_assets -> Failed to create bundle"
        );
        _bundle->SetGraphicsRootSignature(_root_signature.Get());
        _bundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _bundle->IASetVertexBuffers(0, 1, &_vertex_buffer_view);
        _bundle->DrawInstanced(3, 1, 0, 0);
        DX_ASSERT(
            _bundle->Close()
            , "DX12_Renderer::_load_assets -> Failed to close bundle command list."
        );
    }

    {
        DX_ASSERT(
            _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))
            , "DX12_Renderer::_load_assets -> Failed to create fence"
        );

        _state.fence_value = 1;

        _fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (_fence_event == nullptr) {
            DX_ASSERT(HRESULT_FROM_WIN32(GetLastError()), "DX12_Renderer::_load_assets -> invalid fence event.");
        }
    }

    Logger::get()->info("Assets loaded properly");
}

void DX12_Renderer::present() {
    // const float speed = 0.015f;
    // const float offset_bounds = 1.25f;

    // _constant_buffer_data.offset.x += speed;
    // if (_constant_buffer_data.offset.x > offset_bounds) {
    //     _constant_buffer_data.offset.x -= offset_bounds;
    // }
    // memcpy(_p_cbv_data_begin, &_constant_buffer_data, sizeof(_constant_buffer_data));

    // _populate_command_buffers();

    // ID3D12CommandList* pp_command_lists[] = { _command_list.Get() };
    // _command_queue->ExecuteCommandLists(_countof(pp_command_lists), pp_command_lists);

    // DX_ASSERT(
    //     _swapchain->Present(1, 0)
    //     , "DX12_Renderer::_on_render -> Failed to present swapchain image"
    // );

    // _wait_for_prev_frame();
}

void DX12_Renderer::_populate_command_buffers() {
    DX_ASSERT(
        _command_allocator->Reset()
        , "DX12_Renderer::_populate_command_buffers -> Failed to reset command allocator"
    );

    DX_ASSERT(
        _command_list->Reset(_command_allocator.Get(), _pipeline_state.Get())
        , "DX12_Renderer::_populate_command_buffers -> Failed to reset command list"
    );

    _command_list->SetGraphicsRootSignature(_root_signature.Get());

    ID3D12DescriptorHeap* pp_heaps[] = { _cbv_heap.Get() };
    _command_list->SetDescriptorHeaps(_countof(pp_heaps), pp_heaps);

    _command_list->SetGraphicsRootDescriptorTable(0, _cbv_heap->GetGPUDescriptorHandleForHeapStart());
    _command_list->RSSetViewports(1, &_viewport);
    _command_list->RSSetScissorRects(1, &_scissor_rect);

    auto barrier0 = CD3DX12_RESOURCE_BARRIER::Transition(
        _render_targets[_state.frame_index].Get()
        , D3D12_RESOURCE_STATE_PRESENT
        , D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    _command_list->ResourceBarrier(
        1
        , &barrier0
    );

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(_rtv_heap->GetCPUDescriptorHandleForHeapStart(), _state.frame_index, _rtv_descriptor_size);
    _command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);
    
    auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
        _render_targets[_state.frame_index].Get()
        , D3D12_RESOURCE_STATE_PRESENT
        , D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    const f32 clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    _command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
    _command_list->ExecuteBundle(_bundle.Get());
    
    // _command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // _command_list->IASetVertexBuffers(0, 1, &_vertex_buffer_view);
    // _command_list->DrawInstanced(3, 1, 0, 0);
    
    _command_list->ResourceBarrier(
        1
        , &barrier1
    );

    DX_ASSERT(
        _command_list->Close()
        , "DX12_Renderer::_populate_command_buffers -> Failed to close command list after recording commands"
    );
}

void DX12_Renderer::_wait_for_prev_frame() {
    const UINT64 fence = _state.fence_value;
    DX_ASSERT(
        _command_queue->Signal(_fence.Get(), fence)
        , "DX12_Renderer::_wait_for_prev_frame -> Failed to signal fence"
    );
    _state.fence_value++;

    if (_fence->GetCompletedValue() < fence) {
        DX_ASSERT(
            _fence->SetEventOnCompletion(fence, _fence_event)
            , "DX12_Renderer::_wait_for_prev_frame -> Failed to set event on completion"
        );

        WaitForSingleObject(_fence_event, INFINITE);
    }

    _state.frame_index = _swapchain->GetCurrentBackBufferIndex();
}

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WIN32