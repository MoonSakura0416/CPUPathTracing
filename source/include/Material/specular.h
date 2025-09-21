#pragma once


#include "material.h"

class Specular final : public Material {
public:
    explicit Specular(const glm::vec3& albedo) : albedo_(albedo) {}

    glm::vec3 sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi, glm::vec3& beta,
                         const RNG& rng) override;

private:
    glm::vec3 albedo_{};
};
