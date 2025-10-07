#pragma once

#include "light.h"

class InfiniteLight final : public Light {
public:
    explicit InfiniteLight(const glm::vec3& emission) : Light(emission) {}

    [[nodiscard]] std::optional<LightSample>
    lightSample(const glm::vec3& surfacePoint, float sceneRadius, const RNG& rng) const override;

    [[nodiscard]] float Phi(float radius) const override;
};
