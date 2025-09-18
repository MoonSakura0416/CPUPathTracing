#pragma once

#include "pch.h"

#include "Camera/ray.h"
#include "Util/constants.h"

struct  AABB {
    AABB() :min(Infinity), max(-Infinity) {}

    AABB(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max) {}

    [[nodiscard]] bool hasIntersection(const Ray& ray, float tMin, float tMax) const;

    void expand(const glm::vec3& pos)
    {
        min = glm::min(min, pos);
        max = glm::max(max, pos);
    }

    void expand(const AABB& other) {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }

    [[nodiscard]] glm::vec3 diagonal() const
    {
        return max - min;
    }

    [[nodiscard]] float surfaceArea() const
    {
        auto diag = diagonal();
        return 2.f * (diag.x * diag.y + diag.y * diag.z + diag.z * diag.x);
    }

    glm::vec3 min;
    glm::vec3 max;



};