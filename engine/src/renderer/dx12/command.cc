#include "renderer/dx12/renderer.h"

namespace gravity {
namespace renderer {
namespace dx12 {

DXCommand::DXCommand(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type) {
    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.NodeMask = 0;
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queue_desc.Type = type;

    DX_ASSERT(device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&_queue))
        , "DXCommand Failed to create queue"
    );
    NAME_DX12_OBJ(
        _queue
        , type == D3D12_COMMAND_LIST_TYPE_DIRECT ? 
            L"DIRECT COMMAND QUEUE"
        : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? 
            L"COMPUTE COMMAND QUEUE"
        : type == D3D12_COMMAND_LIST_TYPE_COPY ? 
            L"COPY COMMAND QUEUE"
        : type == D3D12_COMMAND_LIST_TYPE_BUNDLE ? 
            L"BUNDLE COMMAND QUEUE"
        : 
            L"TYPE UNKNOWN QUEUE"
    );

    for (
        u32 i = 0
        ; i < NUM_FRAMES
        ; i++
    ) {
        command_frame& frame { _cmd_frames[i] };
        DX_ASSERT(
            device->CreateCommandAllocator(type, IID_PPV_ARGS(&frame.allocator))
            , "DXCommand Failed to create command allocator for command frame."
        );
        wchar_t buf[30];
        swprintf(buf, L"GFX Command Allocator [%u]", i);
        NAME_DX12_OBJ(frame.allocator, buf);
    }

    UINT node_mask = 0;
    DX_ASSERT(device->CreateCommandList(
            node_mask
            , type
            , _cmd_frames[0].allocator
            , nullptr
            , IID_PPV_ARGS(&_list)
        )
        , "DXCommand failed to create command list"
    );
    NAME_DX12_OBJ(
        _list
        , type == D3D12_COMMAND_LIST_TYPE_DIRECT ? 
            L"DIRECT COMMAND LIST"
        : type == D3D12_COMMAND_LIST_TYPE_COMPUTE ? 
            L"COMPUTE COMMAND LIST"
        : type == D3D12_COMMAND_LIST_TYPE_COPY ? 
            L"COPY COMMAND LIST"
        : type == D3D12_COMMAND_LIST_TYPE_BUNDLE ? 
            L"BUNDLE COMMAND LIST"
        : 
            L"TYPE UNKNOWN LIST"
    );

    DX_ASSERT(
        device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))
        , "DXCommand failed to create fence."
    );
    NAME_DX12_OBJ(_fence, L"D3D12 Fence");

    _fence_event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
    assert(_fence_event);

    _list->Close();
}

/// @brief Check that all objects have been properly released
DXCommand::~DXCommand()
{
    assert(
        !_fence
        && !_list
        && !_queue
    );
}

/// @brief Reset our command list for recording
void DXCommand::begin_frame() {
    command_frame& frame { _cmd_frames[_frame_index] };
    frame.wait(_fence_event, _fence);
    DX_ASSERT(frame.allocator->Reset(), "DXCommand Failed to reset command allocator in begin_frame.");
    DX_ASSERT(_list->Reset(frame.allocator, nullptr), "DXCommand Failed to reset command list in begin_frame.");
}

/// @brief Execute the recorded commands and signal our fence
void DXCommand::end_frame() {
    DX_ASSERT(_list->Close(), "DXCommand failed to close command list in end_frame.");
    ID3D12CommandList *const lists[] { _list };
    _queue->ExecuteCommandLists(_countof(lists), &lists[0]);

    u64& fence_value { _fence_value };
    ++fence_value;
    command_frame& frame { _cmd_frames[_frame_index] };
    frame.fence_value = fence_value;
    _queue->Signal(_fence, fence_value);

    _frame_index = (_frame_index + 1) % NUM_FRAMES;
}

/// @brief Release all COM objects held by this object
void DXCommand::release() {
    _flush();
    dx12::release(_fence);
    _fence_value = 0;

    if (_fence_event) {
        CloseHandle(_fence_event);
        _fence_event = nullptr;
    }

    dx12::release(_queue);
    dx12::release(_list);
    
    for (u32 i = 0; i < NUM_FRAMES; i++) 
        { _cmd_frames[i].release(); }
}

void DXCommand::_flush() {
    for (u32 i = 0; i < NUM_FRAMES; i++) 
    {
        _cmd_frames[i].wait(_fence_event, _fence);
    }
    _frame_index = 0;
}

} // dx12 namespace
} // renderer namespace
} // gravity namespace