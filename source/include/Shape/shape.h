#pragma once

#include "pch.h"

#include "Camera/ray.h"
#include "Util/constants.h"
#include "Accelerate/aabb.h"

struct ShapeSample {
    glm::vec3 point;
    glm::vec3 normal;
    float pdf;
};

struct Shape {
    virtual ~Shape() = default;

    [[nodiscard]] virtual std::optional<HitInfo> intersect(const Ray& ray, float tMin = Epsilon,
                                                           float tMax = Infinity) const = 0;

    [[nodiscard]] virtual AABB getBound() const
    {
        return {};
    };

    [[nodiscard]] virtual float getArea() const
    {
        return -1.f;
    }

    [[nodiscard]] virtual std::optional<ShapeSample> shapeSample(const RNG& rng) const
    {
        return std::nullopt;
    }

    [[nodiscard]] virtual float PDF(const glm::vec3& point, const glm::vec3& normal) const
    {
        return 1.f / getArea();
    }
};
