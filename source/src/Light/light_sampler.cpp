#include "Light/light_sampler.h"

void LightSampler::build(float sceneRadius)
{
    std::vector<float> phis;
    phis.reserve(lights_.size());
    for (const auto& light : lights_) {
        phis.push_back(light.get().Phi(sceneRadius));
    }
    aliasTable_.build(phis);
}

std::optional<LightSourceSample> LightSampler::sample(float u) const
{
    if (lights_.empty()) {
        return std::nullopt;
    }
    auto [index, prob] = aliasTable_.sample(u);
    return LightSourceSample { lights_[index], prob };
}
