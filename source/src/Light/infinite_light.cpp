#include "Light/infinite_light.h"
#include "Sample/spherical.h"

std::optional<LightSample> InfiniteLight::lightSample(const glm::vec3& surfacePoint,
                                                      float sceneRadius, const RNG& rng) const
{
    glm::vec3 lightDirection = UniformSampleSphere(rng);
    return LightSample{surfacePoint + 2.f * sceneRadius * lightDirection, lightDirection,
                       emission_, 1.f / (4.f * Pi)};
}

float InfiniteLight::Phi(float radius) const
{
    const float Y = 0.2126f * emission_.r + 0.7152f * emission_.g + 0.0722f * emission_.b;
    return 4 * Pi * Pi * radius * radius * Y;
}
