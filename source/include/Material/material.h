#pragma once

#include "pch.h"

#include "Util/rng.h"

struct BSDFSample {
    glm::vec3 bsdf;
    float     pdf;
    glm::vec3 lightDir;
};

class Material {
public:
    virtual ~Material() = default;

    [[nodiscard]] virtual std::optional<BSDFSample> sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                                 const RNG& rng) const = 0;

    [[nodiscard]] virtual glm::vec3 BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                                         const glm::vec3& viewDir) const = 0;

    [[nodiscard]] virtual bool isDeltaDistribution() const = 0;

public:
    const class AreaLight* areaLight{nullptr};
};
