#include "Material/microfacet.h"
#include "Util/constants.h"
#include "Sample/spherical.h"
#include "Util/frame.h"

Microfacet::Microfacet(float alphaX, float alphaZ)
    : alphaX_(glm::clamp(alphaX * alphaX, 1e-3f, 1.f)),
      alphaZ_(glm::clamp(alphaZ * alphaZ, 1e-3f, 1.f))
{
}

float Microfacet::normalDistribution(const glm::vec3& microfacetNormal) const
{
    glm::vec2 slope{-microfacetNormal.x / microfacetNormal.y,
                    -microfacetNormal.z / microfacetNormal.y};
    slope.x /= alphaX_;
    slope.y /= alphaZ_;
    const float slopeDistribute = slopeDistribution(slope) / (alphaX_ * alphaZ_);
    return slopeDistribute / glm::pow(microfacetNormal.y, 4.f);
}

float Microfacet::masking(const glm::vec3& viewDir, const glm::vec3& microfacetNormal) const
{
    const glm::vec3 viewDirUpper = viewDir.y > 0 ? viewDir : -viewDir;
    if (glm::dot(viewDirUpper, microfacetNormal) <= 0.f) {
        return 0.f;
    }

    return 1.f / (1.f + Lambda(viewDirUpper));
}

float Microfacet::heightCorrelatedMaskingShadowing(const glm::vec3& lightDir,
                                                   const glm::vec3& viewDir,
                                                   const glm::vec3& microfacetNormal) const
{
    const glm::vec3 viewDirUpper = viewDir.y > 0 ? viewDir : -viewDir;
    if (glm::dot(viewDirUpper, microfacetNormal) <= 0.f) {
        return 0.f;
    }

    const glm::vec3 lightDirUpper = lightDir.y > 0 ? lightDir : -lightDir;
    if (glm::dot(lightDirUpper, microfacetNormal) <= 0.f) {
        return 0.f;
    }

    return 1.f / (1.f + Lambda(lightDirUpper) + Lambda(viewDirUpper));
}

float Microfacet::visibleNormalDistribution(const glm::vec3& viewDir,
                                            const glm::vec3& microfacetNormal) const
{
    const glm::vec3 viewDirUpper = viewDir.y > 0 ? viewDir : -viewDir;
    const float     cosThetaO = glm::dot(viewDirUpper, microfacetNormal);
    if (cosThetaO <= 0.f) {
        return 0;
    }
    return normalDistribution(microfacetNormal) * masking(viewDir, microfacetNormal) * cosThetaO /
           std::abs(viewDirUpper.y);
}

glm::vec3 Microfacet::sampleVisibleNormal(const glm::vec3& viewDir, const RNG& rng) const
{
    const glm::vec3 viewDirUpper = viewDir.y > 0 ? viewDir : -viewDir;
    const glm::vec3 viewDirHemi = glm::normalize(
        glm::vec3{alphaX_ * viewDirUpper.x, viewDirUpper.y, alphaZ_ * viewDirUpper.z});
    glm::vec2   sample = UniformSampleUnitDisk({rng.uniform(), rng.uniform()});
    const float h = glm::sqrt(1.f - sample.x * sample.x);
    const float t = 0.5f * (1.f + viewDirHemi.y);
    sample.y = t * sample.y + (1.f - t) * h;

    const Frame     frame(viewDirHemi);
    const glm::vec3 microfacetNormalHemi = frame.worldFromLocal(
        {sample.x, glm::sqrt(1.f - sample.x * sample.x - sample.y * sample.y), sample.y});

    return glm::normalize(glm::vec3(alphaX_ * microfacetNormalHemi.x, microfacetNormalHemi.y,
                                    alphaZ_ * microfacetNormalHemi.z));
}

bool Microfacet::isDeltaDistribution() const
{
    return glm::max(alphaX_, alphaZ_) == 1e-3f;
}

float Microfacet::slopeDistribution(const glm::vec2& slope)
{
    return 1.f / (Pi * glm::pow(slope.x * slope.x + slope.y * slope.y + 1.f, 2.f));
}

float Microfacet::Lambda(const glm::vec3& directionUpper) const
{
    if (directionUpper.y == 0) {
        return Infinity;
    }

    const float length2 = directionUpper.x * directionUpper.x + directionUpper.z * directionUpper.z;
    if (length2 == 0) {
        return 0;
    }

    const float cos2Phi = directionUpper.x * directionUpper.x / length2;
    const float sin2Phi = directionUpper.z * directionUpper.z / length2;
    const float alpha2 = cos2Phi * alphaX_ * alphaX_ + sin2Phi * alphaZ_ * alphaZ_;
    const float tan2Theta = length2 / (directionUpper.y * directionUpper.y);
    return 0.5f * (std::sqrt(1.f + alpha2 * tan2Theta) - 1.f);
}