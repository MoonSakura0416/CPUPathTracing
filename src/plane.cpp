#include "plane.h"


std::optional<HitInfo> Plane::intersect(const Ray& ray, float tMin, float tMax) const
{
    float hitT = glm::dot(point - ray.origin, normal) / glm::dot(ray.direction, normal);
    if (hitT > tMin && hitT < tMax) {
        auto hitPos = ray.at(hitT);
        return HitInfo{hitT, hitPos, normal};
    }
    return std::nullopt;
}