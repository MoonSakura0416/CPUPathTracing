#pragma once

#include "material.h"

class Diffuse final : public Material {
public:
    explicit Diffuse(const glm::vec3& albedo) : albedo_(albedo) {}

    glm::vec3 sampleBRDF(const glm::vec3& wi, glm::vec3& beta, const RNG& rng) override;

private:
    glm::vec3 albedo_{};
};
