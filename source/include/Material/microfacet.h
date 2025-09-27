#pragma once

#include "pch.h"

#include "Util/rng.h"

class Microfacet {
    // Smith Models
    // GGX Distribution
    // Stretching Invariance
public:
    Microfacet(float alphaX, float alphaZ);

    [[nodiscard]] float normalDistribution(const glm::vec3& microfacetNormal) const;

    [[nodiscard]] float masking(const glm::vec3& viewDir, const glm::vec3& microfacetNormal) const;

    [[nodiscard]] float heightCorrelatedMaskingShadowing(const glm::vec3& lightDir,
                                                         const glm::vec3& viewDir,
                                                         const glm::vec3& microfacetNormal) const;

    [[nodiscard]] float visibleNormalDistribution(const glm::vec3& viewDir,
                                                  const glm::vec3& microfacetNormal) const;

    [[nodiscard]] glm::vec3 sampleVisibleNormal(const glm::vec3& viewDir, const RNG& rng) const;

    [[nodiscard]] bool isDeltaDistribution() const;

private:
    [[nodiscard]] static float slopeDistribution(const glm::vec2& slope);
    [[nodiscard]] float        Lambda(const glm::vec3& directionUpper) const;

private:
    float alphaX_{}, alphaZ_{};
};
