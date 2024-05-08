#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <utility>
#include <memory>

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
