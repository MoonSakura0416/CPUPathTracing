#pragma once

#include "material.h"

class Dielectric final: public Material {
public:
    Dielectric(float ior, const glm::vec3& albedo)
        :ior_(ior), albedoR_(albedo), albedoT_(albedo){}

    Dielectric(float ior, const glm::vec3& albedoR, const glm::vec3& albedoT)
        :ior_(ior), albedoR_(albedoR), albedoT_(albedoT){}

    glm::vec3 sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi, glm::vec3& beta,
                         const RNG& rng) override;

private:
    float ior_ {0}; // index of refraction
    glm::vec3 albedoR_{}; // reflectance
    glm::vec3 albedoT_{}; // transmittance
};