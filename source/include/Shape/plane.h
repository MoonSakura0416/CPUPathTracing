#pragma once

#include "shape.h"

struct Plane final : Shape {
    Plane(const glm::vec3& point, const glm::vec3& normal, float radius)
        : point(point), normal(glm::normalize(normal)), radius(radius)
    {
        glm::vec3 up = glm::abs(this->normal.y) < 0.99999 ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1);
        xAxis = glm::normalize(-glm::cross(up, this->normal));
        zAxis = glm::normalize(glm::cross(xAxis, this->normal));

        AABB boundsLocal { { -radius, -0.001, -radius }, { radius, 0.001, radius } };
        for (size_t i = 0; i < 8; i ++) {
            glm::vec3 corner = boundsLocal.getCorner(i);
            bounds.expand(point + corner.x * xAxis + corner.y * normal + corner.z * zAxis);
        }
    }

    [[nodiscard]] AABB getBound() const override
    {
        return bounds;
    }

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

    [[nodiscard]] float                      getArea() const override;

    [[nodiscard]] std::optional<ShapeSample> shapeSample(const RNG& rng) const override;

    glm::vec3 point;
    glm::vec3 normal, xAxis, zAxis;
    AABB bounds;
    float radius;
};
