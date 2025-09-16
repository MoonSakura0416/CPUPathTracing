#pragma once

#include "pch.h"

#include "Camera/ray.h"

struct  AABB {
    AABB() :min(0), max(0) {}

    AABB(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max) {}

    bool hasIntersection(const Ray& ray, float tMin, float tMax) const;

    void expand(const glm::vec3& pos)
    {
        min = glm::min(min, pos);
        max = glm::max(max, pos);
    }

    glm::vec3 min;
    glm::vec3 max;
};