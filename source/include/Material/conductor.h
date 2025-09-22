#pragma once

#include "material.h"

class Conductor final : public Material {
public:
    explicit Conductor(const glm::vec3& ior, const glm::vec3& k) : ior_(ior), k_(k) {}

    glm::vec3 sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi, glm::vec3& beta,
                         const RNG& rng) override;

private:
    glm::vec3 ior_{}, k_{};
};


