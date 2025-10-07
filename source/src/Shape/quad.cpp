#include "Shape/quad.h"

std::optional<HitInfo> Quad::intersect(const Ray& ray, float tMin, float tMax) const
{
    // Calculate the intersection with the plane containing the quad (using the plane formula)
    float hitT = glm::dot(p0 - ray.origin, normal) / glm::dot(ray.direction, normal);

    // Check if the hit is within the valid t range
    if (hitT > tMin && hitT < tMax) {
        // Find the point of intersection
        glm::vec3 hitPos = ray.at(hitT);

        // Check if the hit point is inside the quad (u, v are the barycentric coordinates)
        glm::vec3 v0 = hitPos - p0;
        float     dot00 = glm::dot(v1, v1);
        float     dot01 = glm::dot(v1, v2);
        float     dot02 = glm::dot(v1, v0);
        float     dot11 = glm::dot(v2, v2);
        float     dot12 = glm::dot(v2, v0);

        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        if (u >= 0.f && u <= 1.f && v >= 0.f && v <= 1.f) {
            // The point is inside the quad
            return HitInfo{hitT, hitPos, normal};
        }
    }
    return std::nullopt;
}

float Quad::getArea() const
{
    const float A = glm::length(glm::cross(v1, v2));
    return A;
}

std::optional<ShapeSample> Quad::shapeSample(const RNG& rng) const
{
    const float A = getArea();
    if (A <= 0.0f)
        return std::nullopt;

    const float u = rng.uniform();
    const float v = rng.uniform();

    const glm::vec3 p  = p0 + u * v1 + v * v2;

    const glm::vec3 ns = glm::normalize(normal);

    return ShapeSample{p, ns, 1.0f / A};
}
