#include "Light/area_light.h"

glm::vec3 AreaLight::getEmission(const glm::vec3& surfacePoint, const glm::vec3& lightPoint,
                                 const glm::vec3& normal) const
{
    const glm::vec3 L = surfacePoint - lightPoint;
    const float     cosTheta = glm::dot(normal, L);
    if (!twoSided_ && cosTheta <= 0.f)
        return {};

    return emission_;
}

std::optional<LightSample> AreaLight::lightSample(const glm::vec3&       surfacePoint,
                                                  [[maybe_unused]] float sceneRadius,
                                                  const RNG&             rng) const
{
    const auto shapeSample = shape_->shapeSample(rng);
    if (!shapeSample)
        return std::nullopt;

    const glm::vec3 lightVec = shapeSample->point - surfacePoint;
    const float     r2 = glm::dot(lightVec, lightVec);
    constexpr float eps = 1e-12f;

    if (r2 <= eps)
        return std::nullopt;

    const glm::vec3 wi = lightVec / glm::sqrt(r2);
    const float     cosTheta = glm::dot(shapeSample->normal, -wi);

    if (!twoSided_ && cosTheta <= 0.0f)
        return std::nullopt;

    const float cosThetaAbs = twoSided_ ? glm::abs(cosTheta) : cosTheta;
    if (cosThetaAbs <= eps)
        return std::nullopt;

    // dω/dA = |cos0| / r^2 ⇒ pdf_ω = pdf_A / (dω/dA) = pdf_A * r^2 / |cos0|
    const float detJ = cosThetaAbs / r2;
    const float pdf_omega = shapeSample->pdf / detJ;

    return LightSample{shapeSample->point, wi, emission_, pdf_omega};
}

float AreaLight::Phi(float radius) const
{
    const float Y = 0.2126f * emission_.r + 0.7152f * emission_.g + 0.0722f * emission_.b;
    // return static_cast<float>(twoSided_ ? 2 : 1) * Pi * shape_->getArea() *
    //        std::max({emission_.r, emission_.g, emission_.b});
    return static_cast<float>(twoSided_ ? 2 : 1) * Pi * shape_->getArea() * Y;
}
