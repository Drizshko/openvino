// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <memory>
#include <gtest/gtest.h>
#include <ie_blob.h>

#include "common_test_utils/test_common.hpp"

#include "system_allocator.hpp"

class SystemAllocatorReleaseTests : public CommonTestUtils::TestsCommon {
};

TEST_F(SystemAllocatorReleaseTests, canRelease) {
    SystemMemoryAllocator *allocator_ = new SystemMemoryAllocator();
    allocator_->Release();
}

class SystemAllocatorTests : public CommonTestUtils::TestsCommon {
protected:
    void SetUp() override {
        CommonTestUtils::TestsCommon::SetUp();
        ASSERT_EQ(allocator.get(), nullptr);
        allocator = createSystemMemoryAllocator();
        ASSERT_NE(allocator.get(), nullptr);
    }

    void TearDown() override {
        CommonTestUtils::TestsCommon::TearDown();
        ASSERT_NE(allocator.get(), nullptr);
        allocator.reset();
        ASSERT_EQ(allocator.get(), nullptr);
    }

    std::unique_ptr<SystemMemoryAllocator> createSystemMemoryAllocator() {
        return std::unique_ptr<SystemMemoryAllocator>(new SystemMemoryAllocator());
    }

    std::unique_ptr<SystemMemoryAllocator> allocator;
public:
};

class SystemAllocator : public InferenceEngine::IAllocator {
public:
    using Ptr = std::shared_ptr<SystemAllocator>;

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

        void* ptr = _malloc(size, 4096);

        std::unique_lock<std::mutex> lock(_mutex);
        _size_map[ptr] = size;
        _allocated += size;

        return ptr;
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
        std::unique_lock<std::mutex> lock(_mutex);
        _allocated -= _size_map[ptr];
        _size_map.erase(_size_map.find(ptr));
    }

    size_t allocated() const {
        return _allocated;
    }

private:
    size_t _allocated = 0;
    std::map<void*, size_t> _size_map;
    std::mutex _mutex;
};

TEST_F(SystemAllocatorTests, canAllocate) {
    void *handle0 = allocator->alloc(0);
    void *handle1 = allocator->alloc(100);
    EXPECT_NE(handle0, nullptr);
    EXPECT_NE(handle1, nullptr);
    delete[] reinterpret_cast<char *>(handle0);
    delete[] reinterpret_cast<char *>(handle1);
}

TEST_F(SystemAllocatorTests, canFree) {
    EXPECT_NO_THROW(allocator->free(nullptr));
    void *handle0 = reinterpret_cast<void *>(new char[0]);
    void *handle1 = reinterpret_cast<void *>(new char[100]);
    EXPECT_NO_THROW(allocator->free(handle0));
    EXPECT_NO_THROW(allocator->free(handle1));
}

TEST_F(SystemAllocatorTests, canLockAndUnlockAllocatedMemory) {
    // large block such as 10k will result in sigsegv if not allocated
    void *handle = allocator->alloc(10000);
    char *ptr = reinterpret_cast<char *>(allocator->lock(handle));
    ptr[9999] = 11;
    EXPECT_EQ(ptr[9999], 11);
    allocator->unlock(ptr);
    allocator->free(handle);
}

TEST_F(SystemAllocatorTests, canSetSystemAllocator) {
    std::shared_ptr<SystemAllocator> allocator = shared_from_irelease(new SystemAllocator());
    InferenceEngine::SetSystemAllocator(allocator.get());
    EXPECT_EQ(allocator.get(), InferenceEngine::GetSystemAllocator());
}

TEST_F(SystemAllocatorTests, canCustomAllocatorAllocateAndFree) {
    std::shared_ptr<SystemAllocator> allocator = shared_from_irelease(new SystemAllocator());
    InferenceEngine::SetSystemAllocator(allocator.get());
    auto* sysAllocator = InferenceEngine::GetSystemAllocator();
    void* ptr = sysAllocator->alloc(1024);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(allocator->allocated(), 1024);
    EXPECT_NO_THROW(sysAllocator->free(ptr));
    EXPECT_EQ(allocator->allocated(), 0);
}

TEST_F(SystemAllocatorTests, canCustomAllocatorAllocateBlob) {
    std::shared_ptr<SystemAllocator> allocator = shared_from_irelease(new SystemAllocator());
    InferenceEngine::SetSystemAllocator(allocator.get());

    {
        InferenceEngine::TBlob<float> blob({ InferenceEngine::Precision::FP32, { 1024 }, InferenceEngine::C });
        blob.allocate();
        EXPECT_EQ(allocator->allocated(), 1024 * sizeof(float));
    }

    EXPECT_EQ(allocator->allocated(), 0);

    // shared_from_irelease
}