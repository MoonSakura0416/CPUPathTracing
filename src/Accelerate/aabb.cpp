#include "pch.h"

#include "Accelerate/aabb.h"

bool AABB::hasIntersection(const Ray& ray, float tMin, float tMax) const
{
    // t = (a_x - o.x) / d.x, same for y and z
    glm::vec3 t1 = (min - ray.origin) / ray.direction;
    glm::vec3 t2 = (max - ray.origin) / ray.direction;
    glm::vec3 tmin = glm::min(t1, t2);
    glm::vec3 tmax = glm::max(t1, t2);

    float near = std::max(tMin, std::max(tmin.x, std::max(tmin.y, tmin.z)));
    float far  = std::min(tMax, std::min(tmax.x, std::min(tmax.y, tmax.z)));

    return (near <= far);
}
