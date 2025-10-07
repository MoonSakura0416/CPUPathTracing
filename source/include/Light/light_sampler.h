#pragma once

#include "pch.h"

#include "light.h"
#include "Sample/alias_table.h"

struct LightSourceSample {
    std::reference_wrapper<const Light> light;
    float prob;
};

class LightSampler {
public:
    LightSampler() = default;

    void addLight(const Light& light) {
        lights_.emplace_back(light);
    }

    void build(float sceneRadius);

    [[nodiscard]] std::optional<LightSourceSample> sample(float u) const;

    void clear() noexcept {
        lights_.clear();
    }

private:
    std::vector<std::reference_wrapper<const Light>> lights_;
    AliasTable aliasTable_;
};
