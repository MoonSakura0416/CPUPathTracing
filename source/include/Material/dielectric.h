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
                                         const RNG& rng) override;

private:
    float      ior_{0};     // index of refraction
    glm::vec3  albedoR_{};  // reflectance
    glm::vec3  albedoT_{};  // transmittance
    Microfacet microfacet_;
};