//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#pragma once

#include <cstddef>

#include "ngraph/util.hpp"
#include "ngraph/ngraph_visibility.hpp"

namespace ngraph
{
    namespace runtime
    {
        class Allocator;
    }
}

/// \brief Memory allocator interface.
class NGRAPH_API ngraph::runtime::Allocator
{
public:
    virtual ~Allocator() = default;
public:
    virtual void* alloc(size_t size) = 0;
    virtual void free(void* handle) = 0;
};

namespace ngraph
{
    NGRAPH_API runtime::Allocator* create_default_allocator();

    NGRAPH_API runtime::Allocator* get_system_allocator();

    NGRAPH_API void set_system_allocator(runtime::Allocator*);
}
