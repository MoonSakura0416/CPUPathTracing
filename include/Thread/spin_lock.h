#pragma once

#include "pch.h"

class SpinLock {
public:
    void acquire()
    {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        };
    }

    void release()
    {
        flag_.clear(std::memory_order_release);
    }
private:
    std::atomic_flag flag_{};
};

class Guard {
public:
    explicit Guard(SpinLock& lock) : spinlock_(lock)
    {
        spinlock_.acquire();
    }

    ~Guard()
    {
        spinlock_.release();
    }

private:
    SpinLock& spinlock_;
};
