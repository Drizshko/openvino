// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

/**
 * @brief This is a header file for Inference Engine Extension Interface
 *
 * @file ie_memory_manager.h
 */
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "details/ie_no_copy.hpp"
#include "ie_api.h"
#include "ie_error.hpp"
#include "ie_common.h"
#include "ie_layouts.h"
#include "ie_blob.h"
#include "ie_layers.h"
#include "ie_version.hpp"


namespace InferenceEngine {


/**
 * @brief This class is the main external allocator interface
 */
class INFERENCE_ENGINE_API_CLASS(IExternalAllocator) {
public:
    virtual char* Allocate(size_t size) = 0;
    virtual void Free(char* ptr) = 0;
};

/**
 * @brief A shared pointer to a IExternalAllocator interface
 */
using IExternalAllocatorPtr = std::shared_ptr<IExternalAllocator>;

}  // namespace InferenceEngine
