#pragma once

#include "light.h"

class InfiniteLight final : public Light {
public:
    explicit InfiniteLight(const glm::vec3& emission) : Light(emission) {}

    [[nodiscard]] std::optional<LightSample>
    lightSample(const glm::vec3& surfacePoint, float sceneRadius, const RNG& rng, bool allowMISCompensation) const override;

    [[nodiscard]] float Phi(float radius) const override;

    [[nodiscard]] LightType getType() const override
    {
        return LightType::Infinite;
    }

    [[nodiscard]] float PDF(const glm::vec3& surfacePoint, const glm::vec3& lightPoint,
                            const glm::vec3& normal, bool allowMISCompensation) const override;
};
