#pragma once

#include "pch.h"

#include "Camera/ray.h"
#include "Util/constants.h"
#include "Accelerate/aabb.h"

struct Shape {
    virtual ~Shape() = default;

    [[nodiscard]] virtual std::optional<HitInfo> intersect(const Ray& ray, float tMin = Epsilon,
                                                           float tMax = Infinity) const = 0;

    [[nodiscard]] virtual AABB getBound() const
    {
        return {};
    };
};
