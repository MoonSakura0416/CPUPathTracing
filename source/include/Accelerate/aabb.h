#pragma once

#include "pch.h"

#include "Camera/ray.h"
#include "Util/constants.h"

struct AABB {
    AABB() : min(Infinity), max(-Infinity) {}

    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    [[nodiscard]] bool hasIntersection(const Ray& ray, float tMin, float tMax) const;

    void expand(const glm::vec3& pos)
    {
        min = glm::min(min, pos);
        max = glm::max(max, pos);
    }

    void expand(const AABB& other)
    {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }

    [[nodiscard]] glm::vec3 diagonal() const
    {
        return max - min;
    }

    [[nodiscard]] float surfaceArea() const
    {
        // if aabb is default constructed, return 0
        if (min.x > max.x || min.y > max.y || min.z > max.z) {
            return 0.f;
        }

        auto diag = diagonal();

        return 2.f * (diag.x * diag.y + diag.y * diag.z + diag.z * diag.x);
    }

    // idx from 0 to 7 to get the 8 corners of the AABB
    [[nodiscard]] glm::vec3 getConer(const size_t idx) const
    {
        auto corner = max;
        if (idx & 1)
            corner.x = min.x;
        if (idx & 2)
            corner.y = min.y;
        if (idx & 4)
            corner.z = min.z;
        return corner;
    }

    // Check if the AABB is valid (not default constructed)
    [[nodiscard]] bool isValid() const
    {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    glm::vec3 min;
    glm::vec3 max;
};