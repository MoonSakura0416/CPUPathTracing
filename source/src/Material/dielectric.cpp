#include "Material/dielectric.h"

std::optional<BSDFSample> Dielectric::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                                 const RNG& rng) const
{
    constexpr float eps = 1e-6f;

    if (ior_ == 1.f) {
        // No refraction, treat as perfect mirror
        const glm::vec3 lightDir = -wi;
        constexpr float pdf = 1.f;
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
        // const glm::vec3 lightDir = -wi + 2.f * glm::dot(wi, microfacetNormal) * microfacetNormal;
        const glm::vec3 lightDir = glm::reflect(-wi, microfacetNormal);
        if (microfacet_.isDeltaDistribution()) {
            constexpr float pdf = 1.f;
            const float     cosO = glm::max(std::abs(lightDir.y), eps);
            const glm::vec3 bsdf = albedoR_ / cosO;
            return BSDFSample{bsdf, pdf, lightDir};
        }
        const float pdf = reflectance *
                          microfacet_.visibleNormalDistribution(wi, microfacetNormal) /
                          (4.f * glm::abs(glm::dot(wi, microfacetNormal)));
        const glm::vec3 bsdf =
            reflectance * albedoR_ *
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
    const float pdf =
        (1.f - reflectance) * microfacet_.visibleNormalDistribution(wi, microfacetNormal) * detJ;
    const glm::vec3 bsdf =
        (1.f - reflectance) * albedoT_ * detJ * microfacet_.normalDistribution(microfacetNormal) *
        microfacet_.heightCorrelatedMaskingShadowing(lightDir, wi, microfacetNormal) *
        glm::abs(glm::dot(wi, microfacetNormal) / (lightDir.y * wi.y));
    return BSDFSample{bsdf * etaRatio * etaRatio, pdf, lightDir, etaRatio * etaRatio};
}
glm::vec3 Dielectric::BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                           const glm::vec3& viewDir) const
{
    if (isDeltaDistribution()) {
        return {};
    }

    constexpr float eps = 1e-6f;
    float           lv = lightDir.y * viewDir.y;
    if (lv == 0.f) {
        return {};
    }

    float cosThetaI = glm::clamp(viewDir.y, -1.f, 1.f);
    float etaRatio;
    float scale = 1;
    if (cosThetaI >= 0.0f) {  // outside -> inside
        etaRatio = 1.f / ior_;
    } else {  // inside -> outside
        etaRatio = ior_;
        cosThetaI = -cosThetaI;
        scale = -1;
    }

    float cosThetaT = 0.f;
    float reflectance = FresnelDielectric(etaRatio, cosThetaI, cosThetaT);

    if (lv < 0.f) {
        // BTDF
        glm::vec3 microfacetNormal =
            (lightDir + viewDir * etaRatio) * scale / (etaRatio * cosThetaI - cosThetaT);
        const float detJ =
            etaRatio * etaRatio * glm::abs(glm::dot(lightDir, microfacetNormal)) /
            glm::pow(glm::abs(glm::dot(viewDir, microfacetNormal)) -
                         etaRatio * etaRatio * glm::abs(glm::dot(lightDir, microfacetNormal)),
                     2);
        const glm::vec3 btdf =
            (1 - reflectance) * albedoT_ * detJ * microfacet_.normalDistribution(microfacetNormal) *
            microfacet_.heightCorrelatedMaskingShadowing(lightDir, viewDir, microfacetNormal) *
            glm::abs(glm::dot(viewDir, microfacetNormal) / (lv));
        return btdf;
    }
    // BRDF
    glm::vec3 microfacetNormal = glm::normalize(lightDir + viewDir);
    if (microfacetNormal.y <= 0.f) {
        microfacetNormal = -microfacetNormal;
    }
    const glm::vec3 brdf =
        reflectance * albedoR_ *
        microfacet_.heightCorrelatedMaskingShadowing(lightDir, viewDir, microfacetNormal) *
        microfacet_.normalDistribution(microfacetNormal) / (4.f * glm::abs(lv));
    return brdf;
}

float Dielectric::PDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                      const glm::vec3& viewDir) const
{
    if (isDeltaDistribution()) {
        return 0;
    }

    constexpr float eps = 1e-6f;
    float           lv = lightDir.y * viewDir.y;
    if (lv == 0.f) {
        return 0;
    }

    float cosThetaI = glm::clamp(viewDir.y, -1.f, 1.f);
    float etaRatio;
    float scale = 1;
    if (cosThetaI >= 0.0f) {  // outside -> inside
        etaRatio = 1.f / ior_;
    } else {  // inside -> outside
        etaRatio = ior_;
        cosThetaI = -cosThetaI;
        scale = -1;
    }

    float cosThetaT = 0.f;
    float reflectance = FresnelDielectric(etaRatio, cosThetaI, cosThetaT);

    if (lv < 0.f) {
        // BTDF
        glm::vec3 microfacetNormal =
            (lightDir + viewDir * etaRatio) * scale / (etaRatio * cosThetaI - cosThetaT);
        const float detJ =
            etaRatio * etaRatio * glm::abs(glm::dot(lightDir, microfacetNormal)) /
            glm::pow(glm::abs(glm::dot(viewDir, microfacetNormal)) -
                         etaRatio * etaRatio * glm::abs(glm::dot(lightDir, microfacetNormal)),
                     2);
        return (1.f - reflectance) *
               microfacet_.visibleNormalDistribution(viewDir, microfacetNormal) * detJ;
    }
    // BRDF
    glm::vec3 microfacetNormal = glm::normalize(lightDir + viewDir);
    if (microfacetNormal.y <= 0.f) {
        microfacetNormal = -microfacetNormal;
    }
    return reflectance * microfacet_.visibleNormalDistribution(viewDir, microfacetNormal) /
           (4.f * glm::abs(glm::dot(viewDir, microfacetNormal)));
}
