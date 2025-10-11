#include "Material/ground.h"
#include "Sample/spherical.h"

std::optional<BSDFSample> Ground::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                             const RNG& rng) const
{
    if (wi.y <= 0) {
        return std::nullopt;
    }
    const glm::vec3 lightDir = CosineSampleHemisphere({rng.uniform(), rng.uniform()});
    const float     pdf = CosineSampleHemispherePDF(lightDir);
    glm::vec3       bsdf = albedo_ / Pi;
    if ((static_cast<int>(glm::floor(hitPos.x * 6 + 0.5)) % 6 == 0) ||
        (static_cast<int>(glm::floor(hitPos.z * 6 + 0.5)) % 6 == 0)) {
        bsdf *= 0.1f;
    }
    return BSDFSample{bsdf, pdf, lightDir * glm::sign(wi.y)};
}

glm::vec3 Ground::BSDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                       const glm::vec3& viewDir) const
{
    if (lightDir.y * viewDir.y <= 0) {
        return {};
    }
    glm::vec3       bsdf = albedo_ / Pi;
    if ((static_cast<int>(glm::floor(hitPos.x * 6 + 0.5)) % 6 == 0) ||
        (static_cast<int>(glm::floor(hitPos.z * 6 + 0.5)) % 6 == 0)) {
        bsdf *= 0.1f;
        }
    return bsdf;
}

float Ground::PDF(const glm::vec3& hitPos, const glm::vec3& lightDir,
                  const glm::vec3& viewDir) const
{
    if (lightDir.y * viewDir.y <= 0) {
        return 0;
    }
    return CosineSampleHemispherePDF(lightDir);
}
