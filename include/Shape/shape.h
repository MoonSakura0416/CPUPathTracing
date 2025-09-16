#pragma once

#include "Camera/ray.h"
#include "Util/constants.h"

#include <optional>

struct Shape {
    virtual ~Shape() = default;

    [[nodiscard]] virtual std::optional<HitInfo> intersect(const Ray& ray, float tMin = Epsilon,
                                                           float tMax = Infinity) const = 0;
};
