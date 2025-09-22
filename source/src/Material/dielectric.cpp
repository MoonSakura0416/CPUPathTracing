#include "Material/dielectric.h"

float Fresnel(float etaRatio, float& cosThetaI, float cosThetaT)
{
    const float sin2ThetaT = glm::max(0.0f, 1.0f - cosThetaT * cosThetaT);

    const float sin2ThetaI = sin2ThetaT / (etaRatio * etaRatio);

    if (sin2ThetaI >= 1.0f) {  // total internal reflection
        cosThetaI = 0.0f;
        return 1.0f;
    }

    cosThetaI = glm::sqrt(glm::max(0.0f, 1.0f - sin2ThetaI));

    const float rParallel = (cosThetaI - etaRatio * cosThetaT) / (cosThetaI + etaRatio * cosThetaT);

    const float rPerp = (etaRatio * cosThetaI - cosThetaT) / (etaRatio * cosThetaI + cosThetaT);

    return 0.5f * (rParallel * rParallel + rPerp * rPerp);
}


glm::vec3 Dielectric::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi, glm::vec3& beta,
                                 const RNG& rng)
{
    float cosThetaT = glm::clamp(wi.y, -1.f, 1.f);
    float etaRatio  = ior_;
    glm::vec3 n = {0, 1, 0};
    if (cosThetaT < 0) {
        etaRatio = 1.f / etaRatio;
        cosThetaT = -cosThetaT;
        n = -n;
    }

    float cosThetaI;
    float reflectance = Fresnel(etaRatio, cosThetaI, cosThetaT);

    if (rng.uniform() < reflectance) {
        // Reflect
        beta *= albedoR_ ;
        return {-wi.x, wi.y, -wi.z};
    } else {
        // Refract
        beta *= albedoT_ / (etaRatio * etaRatio);
        return {(-wi / etaRatio) + (cosThetaT / etaRatio - cosThetaI) * n};
    }
}