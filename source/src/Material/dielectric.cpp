#include "Material/dielectric.h"

float FresnelDielectric(float etaRatio, float cosThetaI, float& cosThetaT)
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

std::optional<BSDFSample> Dielectric::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                                 const RNG& rng)
{
    constexpr float eps = 1e-6f;

    if (ior_ == 1.f) {
        // No refraction, treat as perfect mirror
        const glm::vec3 lightDir = -wi;
        constexpr  float     pdf = 1.f;
        const float     cosO = glm::max(glm::abs(lightDir.y), eps);
        const glm::vec3 bsdf = albedoR_ / cosO;
        return BSDFSample{bsdf, pdf, lightDir};
    }

    float     cosThetaI = glm::clamp(wi.y, -1.f, 1.f);
    float     etaRatio;
    float     scale = 1;
    glm::vec3 microfacetNormal{0, 1, 0};
    if (!microfacet_.isDeltaDistribution()) {
        microfacetNormal = microfacet_.sampleVisibleNormal(wi, rng);
    }
    if (cosThetaI >= 0.0f) {  // outside -> inside
        etaRatio = 1.f / ior_;
    } else {  // inside -> outside
        etaRatio = ior_;
        cosThetaI = -cosThetaI;
        scale = -1;
    }

    float cosThetaT = 0.f;
    float reflectance = FresnelDielectric(etaRatio, cosThetaI, cosThetaT);

    if (rng.uniform() < reflectance) {
        // Reflect
        //const glm::vec3 lightDir = -wi + 2.f * glm::dot(wi, microfacetNormal) * microfacetNormal;
        const glm::vec3 lightDir = glm::reflect(-wi, microfacetNormal);
        if (microfacet_.isDeltaDistribution()) {
            constexpr float pdf = 1.f;
            const float     cosO = glm::max(std::abs(lightDir.y), eps);
            const glm::vec3 bsdf = albedoR_ / cosO;
            return BSDFSample{bsdf, pdf, lightDir};
        }
        const float pdf = microfacet_.visibleNormalDistribution(wi, microfacetNormal) /
                          (4.f * glm::abs(glm::dot(wi, microfacetNormal)));
        const glm::vec3 bsdf =
            albedoR_ *
            microfacet_.heightCorrelatedMaskingShadowing(lightDir, wi, microfacetNormal) *
            microfacet_.normalDistribution(microfacetNormal) / (4.f * glm::abs(wi.y * lightDir.y));
        return BSDFSample{bsdf, pdf, lightDir};
    }
    // Refract
    glm::vec3 lightDir = {etaRatio * (-wi) +
                          (etaRatio * cosThetaI - cosThetaT) * scale * microfacetNormal};
    lightDir = glm::normalize(lightDir);
    const float detJ =
        etaRatio * etaRatio * glm::abs(glm::dot(lightDir, microfacetNormal)) /
        glm::pow(glm::abs(glm::dot(wi, microfacetNormal)) -
                     etaRatio * etaRatio * glm::abs(glm::dot(lightDir, microfacetNormal)),
                 2);
    const float     pdf = microfacet_.visibleNormalDistribution(wi, microfacetNormal) * detJ;
    const glm::vec3 bsdf =
        albedoT_ * detJ * microfacet_.normalDistribution(microfacetNormal) *
        microfacet_.heightCorrelatedMaskingShadowing(lightDir, wi, microfacetNormal) *
        glm::abs(glm::dot(wi, microfacetNormal) / (lightDir.y * wi.y));
    return BSDFSample{bsdf * etaRatio * etaRatio, pdf, lightDir};
}