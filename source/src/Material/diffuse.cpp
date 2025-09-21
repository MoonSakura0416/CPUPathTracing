#include "Material/diffuse.h"
#include "Sample/spherical.h"


glm::vec3 Diffuse::sampleBRDF(const glm::vec3& wi, glm::vec3& beta, const RNG& rng)
{
    beta *= albedo_;
    return CosineSampleHemisphere({rng.uniform(), rng.uniform()});
}
