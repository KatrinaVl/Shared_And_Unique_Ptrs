#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <utility>
#include <memory>

class BlockBase {
public:
    virtual ~BlockBase() = default;

    size_t GetStrong() {
        return strong_count_;
    }

    void Increment() {
        ++strong_count_;
    }

    void Decrement() {
        --strong_count_;
        if (strong_count_ == 0) {
            delete this;
        }
    }

    size_t GetWeek() {
        return week_count_;
    }

    size_t strong_count_ = 1;
    size_t week_count_ = 0;
};

template <class T>
class AllocatedByUser : public BlockBase {
public:
    AllocatedByUser(T* ptr) {
        p_ = ptr;
    }

    ~AllocatedByUser() {
        delete p_;
    }

    T* p_;
};

template <class T>
class AllocatedByOurself : public BlockBase {
public:
    template <class... TArgs>
    AllocatedByOurself(TArgs&&... args) {
        new (&p_) T(std::forward<TArgs>(args)...);
    }

    T* Get() {
        return (reinterpret_cast<T*>(p_));
    }

    ~AllocatedByOurself() {
        std::destroy_at(reinterpret_cast<T*>(&p_));
    }

    alignas(T) char p_[sizeof(T)];
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() = default;
    SharedPtr(std::nullptr_t) {
        ptr_ = nullptr;
        block_ = nullptr;
    }

    explicit SharedPtr(T* ptr) {
        ptr_ = ptr;
        block_ = new AllocatedByUser<T>(ptr);
    }

    template <class D>
    explicit SharedPtr(D* ptr) {
        ptr_ = ptr;
        block_ = std::move(new AllocatedByUser(ptr));
    }

    explicit SharedPtr(AllocatedByOurself<T>* ptr) {
        ptr_ = ptr->Get();
        block_ = ptr;
    }

    SharedPtr(const SharedPtr& other) {
        ptr_ = other.Get();
        block_ = other.GetBlock();

        if (block_) {
            block_->Increment();
        }
    }

    template <class D>
    SharedPtr(const SharedPtr<D>& other) {
        ptr_ = other.Get();
        block_ = other.GetBlock();

        if (block_) {
            block_->Increment();
        }
    }

    SharedPtr(SharedPtr&& other) {
        ptr_ = other.ptr_;
        block_ = other.block_;

        if (block_) {
            block_->Increment();
        }

        other.Reset();
    }

    template <typename D>
    SharedPtr(SharedPtr<D>&& other) {
        ptr_ = other.Get();
        block_ = other.GetBlock();

        if (block_) {
            block_->Increment();
        }

        other.Reset();
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        ptr_ = ptr;
        block_ = other.GetBlock();

        if (block_) {
            block_->Increment();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        if (ptr_ == other.Get() && block_ == other.block_){
                    return *this;
                }

        if (block_) {
            block_->Decrement();
        }

        ptr_ = other.ptr_;
        block_ = other.block_;

        if (block_) {
            block_->Increment();
        }

        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        if (ptr_ == other.ptr_ && block_ == other.block_){
            return *this;
        }

        if (block_) {
            block_->Decrement();
        }

        ptr_ = other.ptr_;
        block_ = other.block_;

        if (block_) {
            block_->Increment();
        }

        other.Reset();

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (block_) {
            block_->Decrement();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->Decrement();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    }

    void Reset(T* ptr) {
        if (block_) {
            block_->Decrement();
        }
        ptr_ = ptr;
        block_ = new AllocatedByUser<T>(ptr);
    }

    template <class D>
    void Reset(D* ptr) {
        if (block_) {
            block_->Decrement();
        }
        ptr_ = ptr;
        block_ = new AllocatedByUser<D>(ptr);
    }
    void Swap(SharedPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    }
    BlockBase* GetBlock() const {
        return block_;
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return this->ptr_;
    }
    size_t UseCount() const {
        if (!block_) {
            return 0;
        }
        return block_->GetStrong();
    }
    explicit operator bool() const {
        if (ptr_) {
            return true;
        }
        return false;
    }

private:
    T* ptr_ = nullptr;
    BlockBase* block_ = nullptr;
};

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    auto p = new AllocatedByOurself<T>(std::forward<Args>(args)...);
    SharedPtr<T> ptr(p);

    return ptr;
}
