#pragma once

#include "Thread/spin_lock.h"

class Progress {
public:
    explicit Progress(size_t total, float step = 1.f);


    void update(size_t count);
private:
    size_t total_{0}, current_{0};
    float percent_ {0}, lastPercent_{0}, step_{0};
    SpinLock spinlock_;
};