#pragma once

#include "material.h"
#include "microfacet.h"

class Conductor final : public Material {
public:
    explicit Conductor(const glm::vec3& ior, const glm::vec3& k, float alphaX = 0, float alphaZ = 0)
        : ior_(ior), k_(k), microfacet_(alphaX, alphaZ)
    {
    }

    std::optional<BSDFSample> sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                         const RNG& rng)const override;

    [[nodiscard]] glm::vec3 BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                                 const glm::vec3& viewDir) const override;

    [[nodiscard]] bool      isDeltaDistribution() const override
    {
        return microfacet_.isDeltaDistribution();
    }

    [[nodiscard]] float PDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                            const glm::vec3& viewDir) const override;

private:
    glm::vec3  ior_{}, k_{};
    Microfacet microfacet_;
};
