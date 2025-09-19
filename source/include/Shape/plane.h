#pragma once

#include "shape.h"

struct Plane final : public Shape {
    Plane(const glm::vec3& point, const glm::vec3& normal)
        : point(point), normal(glm::normalize(normal)) {
    }

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

    glm::vec3 point;
    glm::vec3 normal;
};
