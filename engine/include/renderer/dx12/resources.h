#pragma once
#include "core/defines.h"
#include "renderer/renderer.h"
#include "renderer/dx12/fx.h"
#include <mutex>

#if defined(Q_PLATFORM_WINDOWS)

namespace gravity {
namespace renderer {
namespace dx12 {

struct descriptor_handle {
    D3D12_CPU_DESCRIPTOR_HANDLE cpu {};
    D3D12_GPU_DESCRIPTOR_HANDLE gpu {};

    constexpr bool is_valid() const { return cpu.ptr != 0; }
    constexpr bool is_shader_visible() const { return gpu.ptr != 0; }

#if defined(Q_DEBUG)
private:
    friend class DXDescriptorHeap;
    class DXDescriptorHeap* container { nullptr };
    u32 index { };
#endif // Q_DEBUG
};

class DXDescriptorHeap {
public:
    DXDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) : _type{ type } {}
    DISABLE_COPY_AND_MOVE(DXDescriptorHeap)
    ~DXDescriptorHeap() { assert(!_heap); }

    bool initialize(ID3D12Device6* device, u32 capacity, bool shader_visible, class DX12Renderer* parent);
    void release();

    [[nodiscard]] descriptor_handle allocate();
    void free(descriptor_handle& handle);

    void process_deferred_free(u32 frame_index);
    
    constexpr ID3D12DescriptorHeap *const heap() const { return _heap; }
    constexpr D3D12_CPU_DESCRIPTOR_HANDLE cpu_start() const { return _cpu_start; };
    constexpr D3D12_GPU_DESCRIPTOR_HANDLE gpu_start() const { return _gpu_start; };
    constexpr usize capacity() const { return _capacity; }
    constexpr usize size() const { return _size; }
    constexpr usize descriptor_size() const { return _descriptor_size; }
    constexpr const D3D12_DESCRIPTOR_HEAP_TYPE type() const { return _type; }
    constexpr bool is_shader_visible() const { return _gpu_start.ptr != 0; }

private:
    class DX12Renderer* _parent { nullptr };
    ID3D12DescriptorHeap *_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE _cpu_start {};
    D3D12_GPU_DESCRIPTOR_HANDLE _gpu_start {};
    std::unique_ptr<u32[]> _free_handles {};
    std::vector<u32> _deferred_free_indices[NUM_FRAMES];
    std::mutex _mutex {};
    u32 _capacity { 0 };
    u32 _size { 0 };
    u32 _descriptor_size { 0 };
    const D3D12_DESCRIPTOR_HEAP_TYPE _type;
};


} // gravity namespace
} // renderer namespace
} // dx12 namespace

#endif // Q_PLATFORM_WINDOWS