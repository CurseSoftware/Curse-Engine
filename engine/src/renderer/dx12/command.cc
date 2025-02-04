#include "renderer/dx12/dx12_types.h"

#if defined(Q_PLATFORM_WINDOWS)

namespace gravity {
namespace renderer {
namespace dx12 {
using namespace core::logger;

DX12CommandList::DX12CommandList(DX12Device* device) {
    constexpr UINT node_mask = 0;
    DX_ASSERT(
        device->device()->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT
            , IID_PPV_ARGS(&_allocator)
        )
        , "DX12CommandList -> Failed to create command allocator"
    );
    Logger::get()->debug("CommandList created allocator.");
    
    DX_ASSERT(
        device->device()->CreateCommandList(
            node_mask
            , D3D12_COMMAND_LIST_TYPE_DIRECT
            , _allocator.Get()
            , nullptr
            , IID_PPV_ARGS(&_command_list)
        )
        , "DX12CommandList -> Failed to create command list"
    );
    Logger::get()->debug("CommandList created command list.");

    DX_ASSERT(_command_list->Close(), "DX12CommandList -> Failed to close command list");
    
    Logger::get()->debug("CommandList created successfully.");
}

void DX12CommandList::reset() {
    DX_ASSERT(_allocator->Reset(), "DX12CommandList -> Failed to reset allocator");
    DX_ASSERT(_command_list->Reset(_allocator.Get(), nullptr), "DX12CommandList -> Failed to reset command list");
}

} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WINDOWS