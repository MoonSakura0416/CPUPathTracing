#include "pch.h"

#include "Util/progress.h"

Progress::Progress(size_t total, float step) : total_(total), step_(step)
{
    std::println("0%");
}

void Progress::update(size_t count)
{
    Guard guard{spinlock_};

    current_ += count;
    percent_ = 100.f * static_cast<float>(current_) / static_cast<float>(total_);

    if ((percent_ - lastPercent_ >= step_) || (percent_ >= 100.f)) {
        float p = std::clamp(percent_, 0.f, 100.f);

        bool integerStep = false;
        if constexpr (std::is_floating_point_v<decltype(step_)>) {
            integerStep = std::fabs(step_ - std::round(step_)) < 1e-6f;
        } else {
            integerStep = true;
        }

        if (integerStep) {
            std::println("{}%", static_cast<int>(std::lround(p)));
        } else {
            std::println("{:.2f}%", p);
        }

        lastPercent_ = percent_;
    }
}