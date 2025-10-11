#include "Light/light_sampler.h"

void LightSampler::build(float sceneRadius)
{
    std::vector<float> phis;
    phis.reserve(lights_.size());
    for (const auto& light : lights_) {
        phis.push_back(light.get().Phi(sceneRadius));
    }
    aliasTable_.build(phis);
    const auto& probs = aliasTable_.getProbs();
    for (size_t i = 0; i < lights_.size(); i++) {
        lightProbs_.try_emplace(&lights_[i].get(), probs[i]);
    }

    phis.clear();
    for (const auto& light : lightsMIS_) {
        phis.push_back(light.get().Phi(sceneRadius));
    }
    aliasTableMIS_.build(phis);
    const auto& probsMIS = aliasTableMIS_.getProbs();
    for (size_t i = 0; i < lightsMIS_.size(); i++) {
        lightProbsMIS_.try_emplace(&lightsMIS_[i].get(), probsMIS[i]);
    }
}

std::optional<LightSourceSample> LightSampler::sample(float u, bool allowMISCompensation) const
{
    if (allowMISCompensation) {
        if (lightsMIS_.empty()) {
            return std::nullopt;
        }
        auto [index, prob] = aliasTableMIS_.sample(u);
        return LightSourceSample { lightsMIS_[index], prob };
    }

    if (lights_.empty()) {
        return std::nullopt;
    }
    auto [index, prob] = aliasTable_.sample(u);
    return LightSourceSample { lights_[index], prob };
}

float LightSampler::getProb(const Light& light, bool allowMISCompensation) const
{
    if (allowMISCompensation) {
        if (light.getType() == LightType::Infinite) {
            return 0;
        }
        return lightProbsMIS_.at(&light);
    }
    return lightProbs_.at(&light);
}
