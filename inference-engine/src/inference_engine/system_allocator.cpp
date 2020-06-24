// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <mutex>

#include "system_allocator.hpp"
#include "ngraph/runtime/allocator.hpp"

using namespace ngraph;

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
    IAllocator::Ptr _system_AllocatorPtr = shared_from_irelease(CreateDefaultAllocator());
    IAllocator* volatile _system_Allocator = _system_AllocatorPtr.get();

    NGraphAllocator::Ptr _ngraph_AllocatorPtr = NGraphAllocator::Ptr(new NGraphAllocator());
}

IAllocator* GetSystemAllocator() noexcept {
    return _system_Allocator;
}

void SetSystemAllocator(IAllocator* allocator) noexcept {
    if (_system_AllocatorPtr) {
        _system_AllocatorPtr.reset();
    }

    _system_Allocator = allocator;
    set_system_allocator(_ngraph_AllocatorPtr.get());
}

}  // namespace InferenceEngine
