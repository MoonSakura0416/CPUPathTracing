#pragma once

#include "ray.h"
#include "shape.h"

#include <optional>

struct Sphere final : public Shape {
    constexpr Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

    glm::vec3 center;
    float     radius;
};
