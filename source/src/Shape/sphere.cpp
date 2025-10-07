#include "Shape/sphere.h"
#include "Sample/spherical.h"

std::optional<HitInfo> Sphere::intersect(const Ray& ray, float tMin, float tMax) const
{
    // Solve t^2*d.d + 2t*(o-c).d + (o-c).(o-c) - R^2 = 0
    // note d.d = 1
    glm::vec3 co = ray.origin - center;
    float     a = glm::dot(ray.direction, ray.direction);
    float     b = 2 * glm::dot(co, ray.direction);
    float     h = -b / 2;
    float     c = glm::dot(co, co) - radius * radius;
    float     discriminant = h * h - a * c;
    if (discriminant < 0) {
        return std::nullopt;
    }

    float hitT = (h - std::sqrt(discriminant)) / a;

    if (hitT < 0) {
        hitT = (h + std::sqrt(discriminant)) / a;
    }

    if (hitT > tMin && hitT < tMax) {
        auto hitPos = ray.at(hitT);
        auto normal = glm::normalize(hitPos - center);
        return HitInfo{hitT, hitPos, normal};
    }

    return std::nullopt;
}

float Sphere::getArea() const
{
    return 4 * Pi * radius * radius;
}

std::optional<ShapeSample> Sphere::shapeSample(const RNG& rng) const
{
    const glm::vec3 normal = UniformSampleSphere(rng);
    return ShapeSample{center + normal * radius, normal, 1.f / getArea()};
}
