#include <algorithm>

#include "pch.h"

#include "Accelerate/aabb.h"

bool AABB::hasIntersection(const Ray& ray, float tMin, float tMax) const
{
    const glm::vec3 o = ray.origin;
    const glm::vec3 d = ray.direction;

    // For each axis, the "direction is zero" is determined:
    // if the ray does not move on the axis,
    // the origin must fall within the box range of the axis.
    constexpr float EPS = 1e-8f;
    if (std::abs(d.x) < EPS && (o.x < min.x || o.x > max.x)) return false;
    if (std::abs(d.y) < EPS && (o.y < min.y || o.y > max.y)) return false;
    if (std::abs(d.z) < EPS && (o.z < min.z || o.z > max.z)) return false;

    // t = (a_x - o.x) / d.x, same for y and z
    const glm::vec3 invD = 1.0f / d;
    glm::vec3 t1 = (min - o) * invD;
    glm::vec3 t2 = (max - o) * invD;

    const glm::vec3 tmin = glm::min(t1, t2);
    const glm::vec3 tmax = glm::max(t1, t2);

    float tNear = std::max({tMin, tmin.x, tmin.y, tmin.z});
    float tFar  = std::min({tMax, tmax.x, tmax.y, tmax.z});

    return tNear <= tFar;
}
