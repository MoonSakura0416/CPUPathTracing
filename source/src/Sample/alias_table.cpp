#include "Sample/alias_table.h"

void AliasTable::build(const std::vector<float>& values)
{
    if (values.empty()) {
        probs_.clear();
        items_.clear();
        return;
    }

    float sum = 0.0f;
    for (float v : values) {
        assert(v >= 0.0f && "weights must be non-negative");
        sum += v;
    }

    if (sum <= 0.0f) {
        // all zeros -> uniform distribution
        const std::size_t n = values.size();
        probs_.assign(n, 1.0f / static_cast<float>(n));
        items_.assign(n, Item{});
        return;
    }

    const std::size_t n = values.size();
    probs_.resize(n);
    items_.assign(n, Item{});

    std::vector<std::size_t> less, greater;
    less.reserve(n);
    greater.reserve(n);

    for (std::size_t i = 0; i < n; ++i) {
        probs_[i] = values[i] / sum;
        items_[i].q = 1.0f;
        items_[i].p = probs_[i] * static_cast<float>(n);

        if (items_[i].p < 1.0f) {
            less.push_back(i);
        } else if (items_[i].p > 1.0f) {
            greater.push_back(i);
        }
    }

    while (!less.empty() && !greater.empty()) {
        const std::size_t l = less.back();
        less.pop_back();
        const std::size_t g = greater.back();
        greater.pop_back();

        auto& itemL = items_[l];
        auto& itemG = items_[g];

        itemL.q = itemL.p;
        itemL.aliasIndex = g;
        itemG.p -= (1.0f - itemL.q);

        if (itemG.p < 1.0f) {
            less.push_back(g);
        } else if (itemG.p > 1.0f) {
            greater.push_back(g);
        }
    }
}

AliasTable::SampleResult AliasTable::sample(float u) const
{
    const std::size_t n = items_.size();
    assert(n > 0 && "AliasTable::sample: table is empty");

    const float oneMinus = std::nextafter(1.0f, 0.0f);
    u = glm::clamp(u, 0.0f, oneMinus);

    const float scaled = u * static_cast<float>(n);
    const auto  idx = static_cast<std::size_t>(glm::floor(scaled));
    const float frac = scaled - static_cast<float>(idx);

    const auto& item = items_[idx];
    if (frac <= item.q) {
        return {static_cast<int>(idx), probs_[idx]};
    }
    return {static_cast<int>(item.aliasIndex), probs_[item.aliasIndex]};
}