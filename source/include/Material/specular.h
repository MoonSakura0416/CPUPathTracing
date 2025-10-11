#pragma once

#include "material.h"

class Specular final : public Material {
public:
    explicit Specular(const glm::vec3& albedo) : albedo_(albedo) {}

    std::optional<BSDFSample> sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                         const RNG& rng)const override;

    [[nodiscard]] glm::vec3 BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                                 const glm::vec3& viewDir) const override
    {
        return {};
    }

    [[nodiscard]] bool      isDeltaDistribution() const override
    {
        return true;
    }

    [[nodiscard]] float PDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                            const glm::vec3& viewDir) const override
    {
        return 0.f;
    }

private:
    glm::vec3 albedo_{};
};
