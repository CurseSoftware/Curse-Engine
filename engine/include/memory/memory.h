#pragma once
#include "core/defines.h"
#include "core/types.h"

namespace gravity {

namespace memory {

/// @brief Tag for which system or attribute is allocating memory
enum tag {
    UNKNOWN,
    ARRAY,
    LINEAR_ALLOCATOR,
    DARRAY,
    DICT,
    RING_QUEUE,
    BST,
    STRING,
    ENGINE,
    JOB,
    TEXTURE,
    MATERIAL_INSTANCE,
    RENDERER,
    GAME,
    TRANSFORM,
    ENTITY,
    ENTITY_NODE,
    SCENE,
    RESOURCE,
    VULKAN,
    
    // "External" vulkan allocations, for reporting purposes only.
    VULKAN_EXT,
    DIRECT3D,
    OPENGL,
    
    // Representation of GPU-local/vram
    GPU_LOCAL,
    BITMAP_FONT,
    SYSTEM_FONT,
    KEYMAP,
    HASHTABLE,
    UI,
    AUDIO,
    REGISTRY,
    PLUGIN,

    MAX_TAGS
};

class MemorySystem {
public:
    static bool startup();
    static bool shutdown();
    static void incr_tag(tag memory_tag, usize amt);
    static void decr_tag(tag memory_tag, usize amt);
    static void dump_info();
private:
    struct {
        bool is_initialized { false };
        usize tag_memory[tag::MAX_TAGS] {};
    } _state;

    static MemorySystem* instance;
    static MemorySystem* get();
};

template <usize BLOCKSIZE, usize NBLOCKS>
class Pool {
public:
    Pool() {
        usize t_block_size = std::max(BLOCKSIZE, sizeof(block*));
        _block_size = (t_block_size + (alignof(std::max_align_t) - 1)) & ~(alignof(std::max_align_t) - 1);
        _n_blocks = NBLOCKS;
        _blocks_available = _n_blocks;

        _memory = new u8[_block_size * _n_blocks];
        _first_free = reinterpret_cast<block*>(_memory);

        block* curr = _first_free;
        for (usize i = 0; i < _n_blocks; i++) {
            block* next = reinterpret_cast<block*>(reinterpret_cast<u8*>(curr) + _block_size);
            curr->next = next;
            curr = next;
        }

        curr->next = nullptr;
    }
    
    ~Pool() {
        delete[] _memory;
    }

    void* allocate(tag memory_tag) {
        if (!_first_free) {
            return nullptr;
        }

        block* b = _first_free;
        _first_free = b->next;
        _blocks_available -= 1;

        return b;
    }

    void deallocate(void* ptr) {
        if (!ptr) return;

        assert(ptr >= _memory 
            && (_memory + (_block_sizek * _n_blocks)));
        
        block* b = static_cast<block*>(ptr);
        b->next = _first_free;
        _first_free = b;
        _blocks_available += 1;
    }
    
    usize block_size() const { return _block_size; }
    usize block_count() const { return _n_blocks; }
    usize blocks_available() const { return _blocks_available; }
private:
    struct block {
        block* next;
    };
    
    usize _block_size;       // size in bytes for each block
    usize _n_blocks;         // total number of blocks we can allocate
    usize _blocks_available; // number of currently available blocks
    block* _first_free;
    
    u8* _memory;             // where we allocate blocks to

    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
};

} // memory namespace
} // gravity namespace