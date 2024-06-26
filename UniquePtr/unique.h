#pragma once

#include "compressed_pair.h"
#include "deleter.h"

#include <cstddef>  // std::nullptr_t

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : ptr_(ptr, std::move(deleter)) {
    }

    template <class TT, class D>
    UniquePtr(UniquePtr<TT, D>&& other) noexcept
        : ptr_(other.Get(), std::move(other.GetDeleter())) {
        other.Release();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (other.Get() == ptr_.GetFirst()) {
            return *this;
        }
        ptr_.GetSecond()(ptr_.GetFirst());
        ptr_.GetFirst() = other.Get();
        ptr_.GetSecond() = std::move(other.GetDeleter());

        other.Release();
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) {
        ptr_.GetSecond()(ptr_.GetFirst());
        ptr_.GetFirst() = nullptr;

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (ptr_.GetFirst() != nullptr) {
            ptr_.GetSecond()(ptr_.GetFirst());
            ptr_.GetFirst() = nullptr;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* new_ptr = nullptr;
        std::swap(new_ptr, ptr_.GetFirst());
        return new_ptr;
    }
    void Reset(T* ptr = nullptr) noexcept {
        auto old_ptr = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        if (old_ptr) {
            ptr_.GetSecond()(old_ptr);
            old_ptr = nullptr;
        }
    }
    void Swap(UniquePtr& other) {
        std::swap(ptr_.GetFirst(), other.ptr_.GetFirst());
        std::swap(ptr_.GetSecond(), other.ptr_.GetSecond());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_.GetFirst();
    }
    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    }
    explicit operator bool() const {
        if (ptr_.GetFirst()) {
            return true;
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {

        return *ptr_.GetFirst();
    }
    T* operator->() const {
        return this->ptr_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> ptr_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : ptr_(ptr, std::move(deleter)) {
    }

    template <class TT, class D>
    UniquePtr(UniquePtr<TT, D>&& other) noexcept
        : ptr_(other.Get(), std::move(other.GetDeleter())) {
        other.Release();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (other.Get() == this->ptr_.GetFirst()) {
            return *this;
        }

        ptr_.GetSecond()(ptr_.GetFirst());
        ptr_.GetFirst() = other.Get();
        ptr_.GetSecond() = std::move(other.GetDeleter());

        other.Release();
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) {
        ptr_.GetSecond()(ptr_.GetFirst());
        ptr_.GetFirst() = nullptr;

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        if (ptr_.GetFirst() != nullptr) {
            ptr_.GetSecond()(std::move(ptr_.GetFirst()));
            ptr_.GetFirst() = nullptr;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* new_ptr = nullptr;
        std::swap(new_ptr, ptr_.GetFirst());
        return new_ptr;
    }
    void Reset(T* ptr = nullptr) noexcept {
        auto old_ptr = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        if (old_ptr) {
            ptr_.GetSecond()(old_ptr);
            old_ptr = nullptr;
        }
    }
    void Swap(UniquePtr& other) {
        std::swap(ptr_.GetFirst(), other.ptr_.GetFirst());
        std::swap(ptr_.GetSecond(), other.ptr_.GetSecond());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_.GetFirst();
    }
    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    }
    explicit operator bool() const {
        if (ptr_.GetFirst()) {
            return true;
        }
        return false;
    }

    T& operator[](const size_t at) const {
        return *(ptr_.GetFirst() + at);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *ptr_.GetFirst();
    }
    T* operator->() const {
        return this->ptr_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> ptr_;
};
