#pragma once

#include "pch.h"

#include "Camera/ray.h"
#include "shape.h"

struct Sphere final : public Shape {
    constexpr Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

    [[nodiscard]] AABB getBound() const override
    {
        return {center - radius, center + radius};
    }

    glm::vec3 center;
    float     radius;
};
