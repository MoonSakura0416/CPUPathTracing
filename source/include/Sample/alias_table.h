#pragma once

#include "pch.h"

class AliasTable {
private:
    struct Item {
        float q = 1.0f;   // threshold in [0, 1]
        union {
            float p;      // scaled probability during build
            std::size_t aliasIndex; // alias target when q < 1
        };
        Item() : p(0.0f) {}
    };

public:
    struct SampleResult {
        int index;
        float prob;
    };

    AliasTable() = default;

    void build(const std::vector<float>& values);
    [[nodiscard]] SampleResult sample(float u) const;

private:
    std::vector<float> probs_; // normalized probabilities
    std::vector<Item> items_;  // alias table
};
