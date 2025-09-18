#include "Shape/triangle.h"

std::optional<HitInfo> Triangle::intersect(const Ray& ray, float tMin, float tMax) const
{
    // Möller–Trumbore method
    glm::vec3 e1 = p1 - p0;
    glm::vec3 e2 = p2 - p0;
    glm::vec3 s1 = glm::cross(ray.direction, e2);
    float     invDet = 1.f / glm::dot(s1, e1);
    glm::vec3 s = ray.origin - p0;

    float u = glm::dot(s1, s) * invDet;
    if (u < 0 || u > 1) {
        return std::nullopt;
    }

    glm::vec3 s2 = glm::cross(s, e1);
    float     v = glm::dot(s2, ray.direction) * invDet;
    if (v < 0 || u + v > 1) {
        return std::nullopt;
    }

    float hitT = glm::dot(s2, e2) * invDet;
    if (hitT > tMin && hitT < tMax) {
        auto      hitPos = ray.at(hitT);
        glm::vec3 normal = glm::normalize((1 - u - v) * n0 + u * n1 + v * n2);
        return HitInfo{hitT, hitPos, normal};
    }

    return std::nullopt;
}