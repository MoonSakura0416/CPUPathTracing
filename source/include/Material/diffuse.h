#pragma once

#include "material.h"

class Diffuse final : public Material {
public:
    explicit Diffuse(const glm::vec3& albedo = {1,1,1}) : albedo_(albedo) {}

    std::optional<BSDFSample> sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                         const RNG& rng)const override;

    [[nodiscard]] glm::vec3 BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                                 const glm::vec3& viewDir) const override;

    [[nodiscard]] bool      isDeltaDistribution() const override
    {
        return false;
    }

    [[nodiscard]] float PDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                            const glm::vec3& viewDir) const override;

private:
    glm::vec3 albedo_{};
};
