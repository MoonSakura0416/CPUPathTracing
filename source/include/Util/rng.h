#pragma once

#include "pch.h"

class RNG {
public:
    explicit RNG(size_t seed)
    {
        setSeed(seed);
    }

    RNG() :RNG(0){}

    void setSeed(size_t seed)
    {
        gen_.seed(seed);
    }

    float uniform () const
    {
        return dist_(gen_);
    }
private:
    mutable std::mt19937 gen_;
    mutable std::uniform_real_distribution<float> dist_{0,1};
};
