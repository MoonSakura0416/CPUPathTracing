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

    float uniform()
    {
        return dist_(gen_);
    }
private:
    std::mt19937 gen_;
    std::uniform_real_distribution<float> dist_{0,1};
};
