#include "Material/specular.h"
#include "Sample/spherical.h"

std::optional<BSDFSample> Specular::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                               const RNG& rng) const
{
    const glm::vec3 lightDir = {-wi.x, wi.y, -wi.z};
    constexpr float pdf = 1.f;
    const glm::vec3 bsdf = albedo_ / std::abs(lightDir.y);
    return BSDFSample{bsdf, pdf, lightDir};
}