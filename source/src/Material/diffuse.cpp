#include "Material/diffuse.h"

#include "Sample/spherical.h"

std::optional<BSDFSample> Diffuse::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi,
                                              const RNG& rng)
{
    const glm::vec3 lightDir = CosineSampleHemisphere({rng.uniform(), rng.uniform()});
    const float     pdf = CosineSampleHemispherePDF(lightDir);
    const glm::vec3 bsdf = albedo_ / Pi;
    return BSDFSample{bsdf, pdf, lightDir};
}
