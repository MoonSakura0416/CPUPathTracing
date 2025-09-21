#include "Material/specular.h"
#include "Sample/spherical.h"


glm::vec3 Specular::sampleBRDF(const glm::vec3& wi, glm::vec3& beta, const RNG& rng)
{
    beta *= albedo_;
    return {-wi.x, wi.y, -wi.z};
}