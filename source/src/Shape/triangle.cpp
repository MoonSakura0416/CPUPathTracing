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

float Triangle::getArea() const
{
    return 0.5f * glm::length(glm::cross(p1 - p0, p2 - p0));
}

std::optional<ShapeSample> Triangle::shapeSample(const RNG& rng) const
{
    float u = rng.uniform(), v = rng.uniform();
    if (u > v) {
        v *= 0.5;
        u -= v;
    } else {
        u *= 0.5;
        v -= u;
    }

    const float b0 = 1.0f - u - v;
    const float b1 = u;
    const float b2 = v;

    const float A = getArea();
    if (A <= 0.f)
        return std::nullopt;

    const glm::vec3 p = b0 * p0 + b1 * p1 + b2 * p2;
    const glm::vec3 ns = glm::normalize(b0 * n0 + b1 * n1 + b2 * n2);

    return ShapeSample{p, ns, 1.f / A};
}
