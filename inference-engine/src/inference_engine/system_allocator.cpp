// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <mutex>

#include "system_allocator.hpp"

namespace InferenceEngine {

IAllocator* CreateDefaultAllocator() noexcept {
    try {
        return new SystemMemoryAllocator();
    } catch (...) {
        return nullptr;
    }
}

namespace
{
    IAllocator* volatile _system_Allocator = nullptr;
    IAllocator::Ptr _system_AllocatorPtr = nullptr;
    std::mutex _allocator_mutex;
}

IAllocator* GetSystemAllocator() noexcept {
    try {
        if (_system_Allocator == nullptr) {
            std::unique_lock<std::mutex> lock(_allocator_mutex);
            _system_AllocatorPtr = shared_from_irelease(new SystemMemoryAllocator());
            _system_Allocator = _system_AllocatorPtr.get();
        }
    } catch (...) {
    }
    return _system_Allocator;
}

void SetSystemAllocator(IAllocator* allocator) noexcept {
    if (_system_AllocatorPtr) {
        _system_AllocatorPtr.reset();
    }

    _system_Allocator = allocator;
}

}  // namespace InferenceEngine
