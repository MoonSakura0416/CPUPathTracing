#pragma once

#include "material.h"

class Ground final : public Material {
public:
    explicit Ground(const glm::vec3& albedo) : albedo_(albedo) {}

    std::optional<BSDFSample> sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                         const RNG& rng) override;

private:
    glm::vec3 albedo_{};
};
