#include "renderer/dx12/resources.h"
#include "renderer/dx12/renderer.h"

#if defined(Q_PLATFORM_WINDOWS)

namespace gravity {
namespace renderer {
namespace dx12 {

bool DXDescriptorHeap::initialize(
    ID3D12Device6* device
    , u32 capacity
    , bool shader_visible
    , class DX12Renderer* parent
) {
    std::lock_guard lock{ _mutex };
    assert(parent && "Invalid parent set for descriptor heap.");
    _parent = parent;
    assert(capacity && capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);
    assert(!(_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER 
        && capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE));
    assert(device);
    if (
        _type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV
        || _type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV
    ) {
        shader_visible = false;
    }

    release();

    D3D12_DESCRIPTOR_HEAP_DESC desc {};
    desc.Flags = shader_visible
        ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NumDescriptors = capacity;
    desc.Type = _type;
    desc.NodeMask = 0;
    DX_ASSERT(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_heap))
        , "DXDescriptorHeap Failed to create heap."
    );

    _free_handles = std::move(std::make_unique<u32[]>(capacity));
    _capacity = capacity;
    _size = 0;

    for (u32 i = 0; i < capacity; i++) { _free_handles[i] = i; }
    for (u32 i = 0; i < NUM_FRAMES; i++) { _deferred_free_indices[i].clear(); }

    _descriptor_size = device->GetDescriptorHandleIncrementSize(_type);
    _cpu_start = _heap->GetCPUDescriptorHandleForHeapStart();
    _gpu_start = shader_visible  
        ? _heap->GetGPUDescriptorHandleForHeapStart()
        : D3D12_GPU_DESCRIPTOR_HANDLE {0};

    return true;
}

void DXDescriptorHeap::release()
{

}

descriptor_handle DXDescriptorHeap::allocate()
{
    std::lock_guard lock { _mutex };
    assert(_heap);
    assert(_size < _capacity);

    const u32 index { _free_handles[_size] };
    const u32 offset { index * _descriptor_size };
    _size += 1;

    descriptor_handle handle;
    handle.cpu.ptr = _cpu_start.ptr + offset;
    if (is_shader_visible())
    {
        handle.gpu.ptr = _gpu_start.ptr + offset;
    }

#if defined(Q_DEBUG)
    handle.container = this;
    handle.index = index;
#endif // Q_DEBUG

    return handle;
}

void DXDescriptorHeap::free(descriptor_handle& handle)
{
    if (!handle.is_valid()) return;
    std::lock_guard lock { _mutex };
    assert(_heap && _size);
    assert(handle.cpu.ptr >= _cpu_start.ptr);
    assert((handle.cpu.ptr - _cpu_start.ptr) % _descriptor_size == 0);
#if defined(Q_DEBUG)
    assert(handle.container == this);
    assert(handle.index < _capacity);
    const u32 index{(u32)(handle.cpu.ptr - _cpu_start.ptr) / _descriptor_size };
    assert(handle.index == index);
#endif // Q_DEBUG

    const u32 frame_index{ _parent->frame_index()};
    _deferred_free_indices[frame_index].push_back(index);
    _parent->set_deferred_releases_flag();
    handle = {};
}

void DXDescriptorHeap::process_deferred_free(u32 frame_index)
{
    // std::lock_guard(_mutex);
    assert(frame_index < NUM_FRAMES);
    std::vector<u32>& indices { _deferred_free_indices[frame_index] };
    if (!indices.empty())
    {
        for (auto index: indices)
        {
            _size -= 1;
            _free_handles[_size] = index;
        }
        indices.clear();
    }
}


} // dx12 namespace
} // renderer namespace
} // gravity namespace

#endif // Q_PLATFORM_WINDOWS