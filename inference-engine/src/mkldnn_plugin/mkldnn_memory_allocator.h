// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <ie_external_allocator.h>

namespace MKLDNNPlugin {

using Memory = std::shared_ptr<char>;

class MKLDNNMemoryAllocator {
private:
    MKLDNNMemoryAllocator();

public:
    static Memory Allocate(size_t size);

    static void SetExternalAllocator(InferenceEngine::IExternalAllocatorPtr allocator);

private:
    static void CheckInstance();

private:
    static MKLDNNMemoryAllocator* _instance;
    InferenceEngine::IExternalAllocatorPtr _allocator;
};

}  // namespace MKLDNNPlugin
