#pragma once
#include "core/defines.h"
#include "core/logger.h"
#include "fx.h"


namespace gravity {
namespace renderer {
namespace dx12 {

using namespace core::logger;

/// @brief Assertion that DirectX12 API call ended with success
/// @param hr result of call
/// @param err error message for failure
inline void DX_ASSERT(HRESULT hr, const char* err) {
    if (FAILED(hr)) {
        core::logger::Logger::get()->fatal("DirectX Function failed: |%s|", err);
        std::exit(1);
    }
}

template <typename T>
constexpr void release(T*& resource)
{
    if (resource)
    {
        resource->Release();
        resource = nullptr;
    }
}

namespace detail {
    void deferred_release(IUnknown* resource);
}

template <typename T>
constexpr void deferred_release(T*& resource)
{
    if (resource)
    {
        detail::deferred_release(resource);
        resource = nullptr;
    }
}

#if defined(Q_DEBUG)
#define NAME_DX12_OBJ(obj, name) obj->SetName(name);
#else
#define NAME_DX12_OBJ(obj, name)
#endif // Q_DEBUG


} // dx12 namespace
} // renderer namespace
} // gravity namespace