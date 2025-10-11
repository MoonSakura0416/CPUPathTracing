#include "Material/diffuse.h"

#include "Sample/spherical.h"

std::optional<BSDFSample> Diffuse::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                              const RNG& rng) const
{
    if (wi.y <= 0) {
        return std::nullopt;
    }
    const glm::vec3 lightDir = CosineSampleHemisphere({rng.uniform(), rng.uniform()});
    const float     pdf = CosineSampleHemispherePDF(lightDir);
    const glm::vec3 bsdf = albedo_ / Pi;
    return BSDFSample{bsdf, pdf, lightDir * glm::sign(wi.y)};
}

glm::vec3 Diffuse::BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                        const glm::vec3& viewDir) const
{
    if (lightDir.y * viewDir.y <= 0) {
        return {};
    }
    return albedo_ / Pi;
}

float Diffuse::PDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                   const glm::vec3& viewDir) const
{
    if (lightDir.y * viewDir.y <= 0) {
        return 0;
    }
    return CosineSampleHemispherePDF(lightDir);
}
