#include "sphere.h"

std::optional<float> Sphere::intersect(const Ray& ray) const
{
    // Solve t^2*d.d + 2t*(o-c).d + (o-c).(o-c) - R^2 = 0
    // note d.d = 1
    glm::vec3 co = ray.origin - center;
    float     b = glm::dot(co, ray.direction);
    float     c = glm::dot(co, co) - radius * radius;
    float     discriminant = b * b - c;
    if (discriminant < 0) {
        return std::nullopt;
    }

    float hitT = -b - std::sqrt(discriminant);

    if (hitT < 0) {
        hitT = -b + std::sqrt(discriminant);
    }

    if (hitT > 0) {
        return hitT;
    }

    return std::nullopt;
}