#pragma once

#include "shape.h"

struct Quad final : Shape {
    Quad(const glm::vec3& p0, const glm::vec3& v1, const glm::vec3& v2) : p0(p0), v1(v1), v2(v2)
    {
        // Calculate the normal of the quad using the cross product of the edges
        normal = glm::normalize(glm::cross(v1, v2));
    }

    [[nodiscard]] std::optional<HitInfo> intersect(const Ray& ray, float tMin,
                                                   float tMax) const override;

    [[nodiscard]] AABB getBound() const override
    {
        glm::vec3 p1 = p0 + v1;
        glm::vec3 p2 = p0 + v2;
        glm::vec3 p3 = p0 + v1 + v2;

        glm::vec3 minPoint = glm::min(p0, p1);
        glm::vec3 maxPoint = glm::max(p0, p1);

        minPoint = glm::min(minPoint, p2);
        maxPoint = glm::max(maxPoint, p2);
        minPoint = glm::min(minPoint, p3);
        maxPoint = glm::max(maxPoint, p3);

        return {minPoint, maxPoint};
    }

    glm::vec3 p0;      // One corner of the quad
    glm::vec3 v1;      // First edge vector
    glm::vec3 v2;      // Second edge vector
    glm::vec3 normal;  // Normal of the quad
};