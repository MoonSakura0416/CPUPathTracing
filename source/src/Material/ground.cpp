#include "Material/ground.h"
#include "Sample/spherical.h"

glm::vec3 Ground::sampleBSDF(const glm::vec3& hitPos, const glm::vec3& wi, glm::vec3& beta,
                             const RNG& rng)
{
    beta *= albedo_;
    if ((static_cast<int>(glm::floor(hitPos.x * 6)) % 6 == 0) ||
        (static_cast<int>(glm::floor(hitPos.z * 6)) % 6 == 0)) {
        beta *= 0.1f;
    }
    return CosineSampleHemisphere({rng.uniform(), rng.uniform()});
}
