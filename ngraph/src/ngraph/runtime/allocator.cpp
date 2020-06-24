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

#include <memory>

#include "ngraph/runtime/allocator.hpp"
#include "ngraph/runtime/system_allocator.hpp"

using namespace ngraph;
using namespace std;


ngraph::runtime::Allocator* ngraph::create_default_allocator() {
    try {
        return new ngraph::runtime::SystemAllocator();
    } catch (...) {
        return nullptr;
    }
}

namespace
{
    ngraph::runtime::Allocator* volatile _system_allocator = nullptr;
    std::shared_ptr<ngraph::runtime::Allocator> _system_allocator_ptr = nullptr;
    std::mutex _allocator_mutex;
}

ngraph::runtime::Allocator* ngraph::get_system_allocator() {
    try {
        if (_system_allocator == nullptr) {
            std::unique_lock<std::mutex> lock(_allocator_mutex);
            _system_allocator_ptr = shared_ptr<ngraph::runtime::Allocator>(create_default_allocator());
            _system_allocator = _system_allocator_ptr.get();
        }
    } catch (...) {
    }
    return _system_allocator;
}

void ngraph::set_system_allocator(ngraph::runtime::Allocator* allocator) {
    if (_system_allocator_ptr) {
        _system_allocator_ptr.reset();
    }
    _system_allocator = allocator;
}
