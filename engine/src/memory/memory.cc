#include "memory/memory.h"
#include "core/logger.h"

namespace gravity {
namespace memory {
using namespace core::logger;

MemorySystem* MemorySystem::instance = nullptr;

/// @brief Startup for memory subsystem
/// @return true if successful false otherwise
bool MemorySystem::startup() {
    if (instance) {
        Logger::get()->error("Memory system startup attempted on initialized system.");
        return false;
    }

    instance = new MemorySystem();
    usize tags[tag::MAX_TAGS];
    instance->_state = {
        .is_initialized = true,
        .tag_memory = {}
    };

    for (usize i = 0; i < tag::MAX_TAGS; i++) {
        instance->_state.tag_memory[i] = 0;
    }

    return true;
}

/// @brief Shutdown behavior for memory subsystem
/// @return true if successful false otherwise
bool MemorySystem::shutdown() {
    if (!instance) {
        Logger::get()->error("Attempting shutdown on uninitialized memory system.");
        return false;
    }

    delete instance;
    instance = nullptr;
    return true;
}

/// @brief Get singleton of memory subsystem
/// @return Pointer to the singleton
MemorySystem* MemorySystem::get() {
    if (!instance) {
        Logger::get()->fatal("Unable to get() uninitialized MemorySystem.");
        exit(1);
    }

    return instance;
}

/// @brief incriment the amount of bytes that have been allocate for a tag
/// @param memory_tag tag we are allocating to
/// @param amt the amount of bytes that were allocated
void MemorySystem::incr_tag(tag memory_tag, usize amt) {
    get()->_state.tag_memory[memory_tag] += amt;
}

/// @brief decriment the amount of bytes that have been allocate for a tag
/// @param memory_tag tag we are allocating to
/// @param amt the amount of bytes that were allocated
void MemorySystem::decr_tag(tag memory_tag, usize amt) {
    get()->_state.tag_memory[memory_tag] -= amt;
}

/// @brief Print info for each tag
void MemorySystem::dump_info() {
    // TODO:
}

} // memory namespace
} // gravity namespace