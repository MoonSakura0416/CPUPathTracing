#include "Shape/plane.h"
#include "Sample/spherical.h"

std::optional<HitInfo> Plane::intersect(const Ray& ray, float tMin, float tMax) const
{
    float hitT = glm::dot(point - ray.origin, normal) / glm::dot(ray.direction, normal);
    glm::vec3 hitPointToCenter = ray.at(hitT) - point;
    if ((hitT > tMin && hitT < tMax) && (glm::dot(hitPointToCenter, hitPointToCenter) < radius * radius)) {
        auto hitPos = ray.at(hitT);
        return HitInfo{hitT, hitPos, normal};
    }
    return std::nullopt;
}

float Plane::getArea() const
{
    return Pi * radius * radius;
}

std::optional<ShapeSample> Plane::shapeSample(const RNG& rng) const
{
    glm::vec2 sampleLocal = UniformSampleUnitDisk({ rng.uniform(), rng.uniform() }) * radius;
    glm::vec3 samplePoint = point + sampleLocal.x * xAxis + sampleLocal.y * zAxis;
    return ShapeSample { samplePoint, normal, 1.f / getArea() };
}
