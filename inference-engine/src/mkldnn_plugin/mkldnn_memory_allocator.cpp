// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <mkldnn_memory_allocator.h>

namespace MKLDNNPlugin {

MKLDNNMemoryAllocator* MKLDNNMemoryAllocator::_instance = nullptr;

MKLDNNMemoryAllocator::MKLDNNMemoryAllocator() {
}

Memory MKLDNNMemoryAllocator::Allocate(size_t size) {
    CheckInstance();

    Memory handle;

    if (_instance->_allocator) {
        char* ptr = _instance->_allocator->Allocate(size);

        auto _free = [](char* p) {
            _instance->_allocator->Free(p);
        };

        handle.reset(reinterpret_cast<char*>(ptr), _free);
    } else {
        auto _malloc = [](size_t size, int alignment) {
            void *ptr;
#ifdef _WIN32
            ptr = _aligned_malloc(size, alignment);
            int rc = ((ptr)? 0 : errno);
#else
            int rc = ::posix_memalign(&ptr, alignment, size);
#endif /* _WIN32 */
            return (rc == 0) ? reinterpret_cast<char*>(ptr) : nullptr;
        };
        auto _free = [](void* p) {
#ifdef _WIN32
            _aligned_free(c);
#else
            ::free(p);
#endif /* _WIN32 */
        };

        handle.reset(_malloc(size, 4096), _free);
    }

    return handle;
}

void MKLDNNMemoryAllocator::SetExternalAllocator(InferenceEngine::IExternalAllocatorPtr allocator) {
    CheckInstance();
    _instance->_allocator = allocator;
}

void MKLDNNMemoryAllocator::CheckInstance() {
    if (!_instance) {
        _instance = new MKLDNNMemoryAllocator();
    }
}

}  // namespace MKLDNNPlugin
