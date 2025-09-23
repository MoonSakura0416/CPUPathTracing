#include "Material/dielectric.h"

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


std::optional<BSDFSample> Dielectric::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                 const RNG& rng)
{
    constexpr float eps = 1e-6f;

    float cosThetaI = glm::clamp(wi.y, -1.f, 1.f);
    float etaRatio;
    glm::vec3 n = {0, 1, 0};
    if (cosThetaI >= 0.0f) { // outside -> inside
        etaRatio = 1.f / ior_;
    } else { // inside -> outside
        etaRatio = ior_;
        cosThetaI = -cosThetaI;
        n = -n;
    }

    float cosThetaT = 0.f;
    float reflectance = FresnelDielectric(etaRatio, cosThetaI, cosThetaT);

    if (rng.uniform() < reflectance) {
        // Reflect
        const glm::vec3 lightDir = {-wi.x, wi.y, -wi.z};
        constexpr float pdf = 1.f;
        const float cosO = glm::max(std::abs(lightDir.y), eps);
        const glm::vec3       bsdf = albedoR_ / cosO;
        return BSDFSample{bsdf, pdf, lightDir};
    } else {
        // Refract
        glm::vec3 lightDir = {etaRatio * (-wi) + (etaRatio * cosThetaI - cosThetaT) * n};
        lightDir = glm::normalize(lightDir);
        constexpr float pdf = 1.f;
        const float cosO = glm::max(std::abs(lightDir.y), eps);
        const glm::vec3 bsdf = albedoT_ * etaRatio * etaRatio / cosO;
        return BSDFSample{bsdf, pdf, lightDir};
    }
}