#pragma once

#include "shape.h"

struct Triangle final : public Shape {
    constexpr Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
                       const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2)
        : p0(p0), p1(p1), p2(p2), n0(n0), n1(n1), n2(n2)
    {
    }

    Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) : p0(p0), p1(p1), p2(p2)
    {
        glm::vec3 e1 = p1 - p0;
        glm::vec3 e2 = p2 - p0;
        glm::vec3 normal = glm::normalize(glm::cross(e1, e2));
        n0 = normal;
        n1 = normal;
        n2 = normal;
    }

    std::optional<HitInfo> intersect(const Ray& ray, float tMin, float tMax) const override;

    [[nodiscard]] AABB getBound() const override
    {
        AABB bound{};
        bound.expand(p0);
        bound.expand(p1);
        bound.expand(p2);
        return bound;
    }

    [[nodiscard]] float                      getArea() const override;

    [[nodiscard]] std::optional<ShapeSample> shapeSample(const RNG& rng) const override;

    glm::vec3 p0, p1, p2;
    glm::vec3 n0, n1, n2;
};
