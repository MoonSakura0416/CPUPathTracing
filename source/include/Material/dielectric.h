#pragma once

#include "material.h"
#include "microfacet.h"

class Dielectric final : public Material {
public:
    Dielectric(float ior, const glm::vec3& albedo, float alphaX = 0, float alphaZ = 0)
        : ior_(ior), albedoR_(albedo), albedoT_(albedo), microfacet_(alphaX, alphaZ)
    {
    }

    Dielectric(float ior, const glm::vec3& albedoR, const glm::vec3& albedoT, float alphaX = 0,
               float alphaZ = 0)
        : ior_(ior), albedoR_(albedoR), albedoT_(albedoT), microfacet_(alphaX, alphaZ)
    {
    }

    std::optional<BSDFSample> sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                         const RNG& rng)const override;

    [[nodiscard]] glm::vec3 BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                                 const glm::vec3& viewDir) const override;

    [[nodiscard]] bool      isDeltaDistribution() const override
    {
        return ior_ == 1 || microfacet_.isDeltaDistribution();
    }

private:
    float      ior_{0};     // index of refraction
    glm::vec3  albedoR_{};  // reflectance
    glm::vec3  albedoT_{};  // transmittance
    Microfacet microfacet_;
};

inline float FresnelDielectric(float etaRatio, float cosThetaI, float& cosThetaT)
{
    cosThetaI = glm::clamp(cosThetaI, 0.0f, 1.0f);

    // Snell's law: sin^2(theta_t) = sin^2(theta_i) / eta^2
    const float sin2I = glm::max(0.0f, 1.0f - cosThetaI * cosThetaI);
    const float sin2T = sin2I * (etaRatio * etaRatio);

    // Handle total internal reflection
    if (sin2T >= 1.0f) {
        cosThetaT = 0.0f;
        return 1.0f;
    }

    cosThetaT = glm::sqrt(glm::max(0.0f, 1.0f - sin2T));

    // Fresnel reflectance for s- and p-polarized light
    float rsNum = etaRatio * cosThetaI - cosThetaT;
    float rsDen = etaRatio * cosThetaI + cosThetaT;
    float rpNum = cosThetaI - etaRatio * cosThetaT;
    float rpDen = cosThetaI + etaRatio * cosThetaT;

    float rs = (rsNum / rsDen) * (rsNum / rsDen);
    float rp = (rpNum / rpDen) * (rpNum / rpDen);

    return 0.5f * (rs + rp);
}