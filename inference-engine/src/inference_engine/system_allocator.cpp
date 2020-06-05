// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <mutex>

#include "system_allocator.hpp"

namespace InferenceEngine {

namespace
{
    IAllocator* volatile _system_Allocator = nullptr;
    std::mutex _allocator_mutex;
}

IAllocator* GetSystemAllocator() noexcept {
    try {
        if (_system_Allocator == nullptr) {
            std::unique_lock<std::mutex> lock(_allocator_mutex);
            _system_Allocator = new DefaultMemoryAllocator();
        }
    } catch (...) {
    }
    return _system_Allocator;
}

void SetSystemAllocator(IAllocator* allocator) noexcept {
    _system_Allocator = allocator;
}

}  // namespace InferenceEngine
