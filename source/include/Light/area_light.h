#pragma once

#include "light.h"
#include "Shape/shape.h"

class AreaLight final : public Light {
public:
    AreaLight(const Shape* shape, const glm::vec3& emission, bool twoSided)
        : Light(emission), shape_(shape), twoSided_(twoSided)
    {
    }

    [[nodiscard]] glm::vec3 getEmission(const glm::vec3& surfacePoint, const glm::vec3& lightPoint,
                                        const glm::vec3& normal) const override;

    [[nodiscard]] std::optional<LightSample>
    lightSample(const glm::vec3& surfacePoint, float sceneRadius, const RNG& rng, bool allowMISCompensation) const override;

    [[nodiscard]] float Phi(float radius) const override;

    [[nodiscard]] const Shape* getShape() const
    {
        return shape_;
    }

    [[nodiscard]] LightType getType() const override
    {
        return LightType::Area;
    }

    [[nodiscard]] float PDF(const glm::vec3 &surfacePoint, const glm::vec3 &lightPoint, const glm::vec3 &normal, bool allowMISCompensation) const override;

private:
    const Shape* shape_;
    bool         twoSided_;
};
