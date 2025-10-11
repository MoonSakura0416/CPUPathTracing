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
        if (light.getType() != LightType::Infinite) {
            lightsMIS_.emplace_back(light);
        }
    }

    void build(float sceneRadius);

    [[nodiscard]] std::optional<LightSourceSample> sample(float u, bool allowMISCompensation) const;

    void clear() noexcept {
        lights_.clear();
    }

    [[nodiscard]] float getProb(const Light& light, bool allowMISCompensation) const;

private:
    std::vector<std::reference_wrapper<const Light>> lights_;
    std::map<const Light*, float> lightProbs_;
    AliasTable aliasTable_;

    std::vector<std::reference_wrapper<const Light>> lightsMIS_;
    std::map<const Light*, float> lightProbsMIS_;
    AliasTable aliasTableMIS_;
};
