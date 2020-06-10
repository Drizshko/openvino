// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <iostream>
#include <memory>

#include "ie_allocator.hpp"

class SystemMemoryAllocator : public InferenceEngine::IAllocator {
public:
    void Release() noexcept override {
        delete this;
    }

    void* lock(void* handle, InferenceEngine::LockOp = InferenceEngine::LOCK_FOR_WRITE) noexcept override {
        return handle;
    }

    void unlock(void* a) noexcept override {}

    void* alloc(size_t size) noexcept override {
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

        return _malloc(size, 4096);
    }

    void free(void* ptr) noexcept override {
        try {
#ifdef _WIN32
            _aligned_free(ptr);
#else
            ::free(ptr);
#endif /* _WIN32 */
        } catch (...) {
        }
    }
};