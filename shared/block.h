#pragma once

#include "sw_fwd.h"  // Forward declaration

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
